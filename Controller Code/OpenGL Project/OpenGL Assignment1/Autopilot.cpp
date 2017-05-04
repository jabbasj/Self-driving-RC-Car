#include "all_headers.h"

Autopilot::Autopilot(GLRenderer* rend) {
	m_Renderer = rend;
	m_Terrain = &rend->m_Terrain;
	StreetMap = &m_Terrain->StreetMap;
	StopsMap = &m_Terrain->StopsMap;

	position = &m_Renderer->position;
	direction = &m_Renderer->direction;
	PATH_READY = false;
	FOLLOWING_PATH = false;

#ifdef USE_XBEE
	std::cout << "Try xbee...\n";
	try {
		xbee = new libxbee::XBee("xbee1", "COM4", 115200);

		struct xbee_conAddress address;
		memset(&address, 0, sizeof(address));
		address.addr16_enabled = 1;
		// target: 0x0001
		address.addr16[0] = 0x00;
		address.addr16[1] = 0x01;

		m_ConnectionManager = new ConnectionManager(*xbee, "16-bit Data", &address); /* with a callback */
	}
	catch (xbee_err err) {
		printf("xbee_setup(): %d - %s\n", err, xbee_errorToStr(err));
		return;
	}
#else
	m_ConnectionManager = new ConnectionManager();
#endif

	CONNECTION_ACTIVE = m_ConnectionManager->initializeConnection();

	if (CONNECTION_ACTIVE) {
		std::cout << "Connected!\n";
	}
	else {
		std::cout << "NOT FOUND!\n";
	}

}

Autopilot::~Autopilot() {

	delete xbee;
	//delete m_ConnectionManager;
}


void Autopilot::set_start(glm::vec3 pos) {
	
	if (!FOLLOWING_PATH) {
		PATH_READY = false;
		start = scale_position(pos);
		path.clear();

		if (point_meets_threshold(start)) {
			std::cout << "START SET\n";
		}
		else {
			std::cout << "Invalid, try again...\n";
		}
	}
}

void Autopilot::SPEAK(std::string str, bool force /*= false*/) {

	static double lastTime = 0;

	// Time since last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	if (deltaTime > 3 || force) {

		if (str != "" && str.size() > 0) {
			m_Speaker.Speak(str.c_str());
		}
		lastTime = currentTime;
	}	
}


void Autopilot::set_destination(glm::vec3 pos) {

	if (!FOLLOWING_PATH) {

		PATH_READY = false;
		destination = scale_position(pos);

		if (point_meets_threshold(destination)) {
			std::cout << "DESTINATION SET\n";

			generate_path();

			PATH_READY = true;
		}
		else {
			std::cout << "Invalid, try again...\n";
		}
	}
}


void Autopilot::start_autopilot() {

	if (!FOLLOWING_PATH) {
		if (PATH_READY) {
			fp = std::thread(&Autopilot::follow_path, this);
			fp.detach();

			SPEAK("AUTOPILOT STARTING");
			SEND_StartMessage();
		}
		else {
			std::cout << "Path not yet ready!\n";
		}
	}
}

void Autopilot::stop_autopilot() {

	if (FOLLOWING_PATH) {

		std::cout << "Autopilot stopping...\n";

		FOLLOWING_PATH = false;
		if (fp.joinable()) {
			fp.join();
		}

		path.clear();
		SEND_FinishMessage();

		SPEAK("AUTOPILOT STOPPING");
		std::cout << "Autopilot Stopped!\n";		
	}
	else {
		std::cout << "Nothing to stop!\n";
	}

}

void Autopilot::follow_path() {

	if (!CONNECTION_ACTIVE || m_ConnectionManager->finished == true) {

#ifndef USE_XBEE
		CONNECTION_ACTIVE = m_ConnectionManager->initializeConnection();
#endif
		if (CONNECTION_ACTIVE) {
			std::cout << "Connected!\n";
			SEND_StartMessage();
		}
		else {
			std::cout << "NOT FOUND!\n";
			return;
		}		
	}
	
	std::cout << "Following path...\n";

	pos = start;
	_vec2 irl_pos;
	_vec2 last_irl_pos = { 0, 0 };
	float distance = 0.0f;
	float actual_distance = 0.0f;
	float remainder = 0.0f;
	size_t turn_pos = 0;
	size_t stop_pos = 0;
	float turn_angle = 0;

	FOLLOWING_PATH = true;
	PATH_READY = false;
	bool SKIP_STOP = false;
	bool STOP_DETECTED = false;
	bool TURN_DETECTED = false;
	bool STOP_PREVIOUSLY_DETECTED = false;
	

	while (path.size() > 0 && FOLLOWING_PATH && !m_ConnectionManager->finished && CONNECTION_ACTIVE) {
			
		float disp = 0.0f;
		int pop = 0;

		
		while (disp == 0) {

			//Check received messages
			std::vector<my_MSG> received_msgs = m_ConnectionManager->myReceive();
			my_MSG * msg;

			for (size_t k = 0; k < received_msgs.size(); k++) {

				msg = &received_msgs[k];
				HANDLE_ReceivedMessage(msg, &disp);

				if (msg->type == "DISCONNECT") {return; }
			}
		}

		//Compute discrete next_pos in path from "continuous" displacement in cm
		_vec2 next_pos = map_scale(disp, &pop, &remainder);
		actual_distance += disp;

		//Translate map displacement approximation to real-size 
		irl_pos = irl_scale(next_pos);
		if (last_irl_pos.x != 0 && last_irl_pos.z != 0) {
			disp = sqrt(pow(last_irl_pos.x - irl_pos.x, 2) + pow(last_irl_pos.z - irl_pos.z, 2));
			distance += disp;
		}
		
		//Where to point at
		float next_angle = compute_angle(1, pos, next_pos, ANGLE_LOOK_AHEAD);

		//Update car angle & location
		update_renderer_location(next_angle, next_pos);

		//Check for incoming turns
		TURN_DETECTED = check_upcoming_turn(1, &turn_pos, &turn_angle);

		if (TURN_DETECTED) {

			if (abs(turn_angle * 180 / 3.14f) > TURN_ANGLE_THRESHOLD) {

				float turn_dist = irl_scale(sqrt(pow((path.end() - turn_pos)->x - pos.x, 2) + pow((path.end() - turn_pos)->z - pos.z, 2)));			
				std::cout << "turn angle of " << turn_angle * 180 / 3.14f << " in " << turn_dist << " cm\n";
				std::string turn_msg = "TURN ";
				if (turn_angle > 0) {
					turn_msg += "RIGHT ";
				}
				else {
					turn_msg += "LEFT ";
				}
				turn_msg += " IN " + std::to_string(int(turn_dist)) + " centimeter";
				SPEAK(turn_msg);

				SEND_TurnMessage(int(turn_angle * 180 / 3.14f), int(turn_dist));
			}
		}

		//Sleep(250);

		//Check for incoming stops
		STOP_DETECTED = check_upcoming_stop(&stop_pos);		

		if (STOP_DETECTED && !STOP_PREVIOUSLY_DETECTED) {

			STOP_PREVIOUSLY_DETECTED = true;

			if (!SKIP_STOP) {

				float stop_dist = irl_scale(sqrt(pow((path.end() - stop_pos)->x - pos.x, 2) + pow((path.end() - stop_pos)->z - pos.z, 2)));
				//expecting stop				
				std::cout << "\nSTOP IN " << stop_dist << "cm\n";
				std::string stop_msg = "STOP IN " + std::to_string(int(stop_dist)) + " centimeter";
				SPEAK(stop_msg, true);

				SEND_StopSignMessage(int(stop_dist));
			}
		}

		if (!STOP_DETECTED && STOP_PREVIOUSLY_DETECTED) {
			//stop passed
			if (!SKIP_STOP) {	

				std::cout << "\nSTOPPING!\n";
				//Sleep(2000);
			}
			SKIP_STOP = !SKIP_STOP;
			STOP_PREVIOUSLY_DETECTED = false;
		}	
		
		// Pop path points travelled
		for (int i = 0; i < pop; i++) {
			if (path.size() == 0) {
				break;
			}
			path.pop_back();
		}

		last_irl_pos = irl_pos;
	}

	SEND_FinishMessage();
	SPEAK("ARRIVED");
	std::cout << "\nArrived! IRL distance travelled: " << distance << "cm\n";
}


void Autopilot::update_renderer_location(float next_angle, _vec2 next_pos) {

	if (pos != next_pos) {
		m_Renderer->horizontalAngle = next_angle;
	}
	position->x = next_pos.x;
	position->z = next_pos.z;
	pos = next_pos;

}



void Autopilot::SEND_FinishMessage() {

	FOLLOWING_PATH = false;

	my_MSG finish_msg;
	finish_msg.type = "FINISH";
	m_ConnectionManager->mySend(finish_msg);

}

void Autopilot::SEND_StartMessage() {

	my_MSG start_msg;
	start_msg.type = "START";
	m_ConnectionManager->mySend(start_msg);
}

void Autopilot::SEND_StopSignMessage(int stop_dist) {

	my_MSG stop_msg;

	stop_msg.type = "STOP_SIGN";
	stop_msg.num = stop_dist;

	m_ConnectionManager->mySend(stop_msg);
}

void Autopilot::SEND_TurnMessage(int turn_angle, int turn_dist) {

	my_MSG turn_msg;

	turn_msg.type = "TURN";
	turn_msg.num = turn_angle;
	turn_msg.unique_id = turn_dist;

	m_ConnectionManager->mySend(turn_msg);
}

void Autopilot::HANDLE_ReceivedMessage(my_MSG* msg, float * disp) {


	if (msg->type == "DISP") {
		*disp += float(msg->num / 1000.0);

	} else if (msg->type == "DISCONNECT") {

		SPEAK("DISCONNECT", true);
		FOLLOWING_PATH = false;
		path.clear();
		m_ConnectionManager->closeConnection();

		Sleep(2000);
		CONNECTION_ACTIVE = m_ConnectionManager->initializeConnection();
	}
	else if (msg->type == "FINISH") {

		FOLLOWING_PATH = false;
		path.clear();
	}
	else if (msg->type == "SPEAK") {
		SPEAK(msg->message, true); 
	}
	else {
		std::cout << "UNKNOWN MSG:: " << msg->type << "\n";
	}

}


bool Autopilot::check_turn_side(size_t index, float* angle) {

	if (path.size() < index) {
		return false;
	}

	_vec2 curr_pos = *(path.end() - index);
	_vec2 next_pos = *(path.end() - index - 1);

	float turn_angle = compute_angle(index, curr_pos, next_pos, TURN_LOOK_AHEAD);
	float next_angle = compute_angle(index, curr_pos, next_pos, ANGLE_LOOK_AHEAD);

	float angle_diff = (next_angle - turn_angle) * 180 / 3.14f;

	if (abs(angle_diff) > TURN_ANGLE_THRESHOLD /* degrees*/ && abs(angle_diff) < 180) {

		_vec2 temp;
		if (path.size() > (index + TURN_LOOK_AHEAD + 1)) {
			temp = *(path.end() - index - TURN_LOOK_AHEAD - 1);
		}
		else {
			temp = *path.begin();
		}

		glm::vec3 _pos(curr_pos.x, 1, curr_pos.z);
		glm::vec3 _next_pos(next_pos.x, 1, next_pos.z);

		glm::vec3 vecTemp = glm::normalize(glm::vec3(temp.x, 1, temp.z) - _pos);
		glm::vec3 vecA = glm::normalize(_next_pos - _pos);

		float target_angle = vecA.x * vecTemp.z - vecA.z * vecTemp.x;

		if (target_angle > 0) {
			//std::cout << "\nTurning right..." << angle_diff;
		}
		else {
			//std::cout << "\nTurning left..." << angle_diff;
		}
		*angle = target_angle;
		return true;
	}

	return false;
}

void Autopilot::generate_path() {

	//Check if Start & Destination are set and on a street
	if (StreetMap->count(start) == 1 && StreetMap->count(destination) == 1) {

		if (StreetMap->find(start)->second < STREET_IDENTIFIER_THRESHOLD
			&& StreetMap->find(destination)->second < STREET_IDENTIFIER_THRESHOLD) {

		}
		else { return; }
	}
	else { return; }

	AStar::Generator generator(m_Terrain->X_SCALAR, m_Terrain->Z_SCALAR);
	// Set 2d map size.
	generator.setWorldSize({ m_Terrain->MAX_X_POS * m_Terrain->X_SCALAR, m_Terrain->MAX_Z_POS * m_Terrain->Z_SCALAR });
	// You can use a few heuristics : manhattan, euclidean or octagonal.
	generator.setHeuristic(AStar::Heuristic::euclidean);
	generator.setDiagonalMovement(true);

	for (auto i = StreetMap->begin(); i != StreetMap->end(); i++) {
		if (i->second >= STREET_IDENTIFIER_THRESHOLD) {
			AStar::Vec2i pos;
			pos.x = int(i->first.x);
			pos.y = int(i->first.z);
			generator.addCollision(pos);
		}
	}

	std::cout << "Generate path ... \n";
	int start_x;
	int start_z;

	if (path.size() == 0) {
		start_x = int(start.x);
		start_z = int(start.z);
	}
	else {
		start_x = int(path.front().x);
		start_z = int(path.front().z);
	}
	
	// This method returns vector of coordinates from target to source.
	auto gen_path = generator.findPath({ start_x, start_z }, { int(destination.x), int(destination.z) });

	//path.clear();
	_vec2 pos;
	std::vector<_vec2> temp_path;	

	for (auto& coordinate : gen_path) {
		//std::cout << coordinate.x << " " << coordinate.y << "\n";
		pos.x = float(coordinate.x); pos.z = float(coordinate.y);

		temp_path.push_back(pos);
	}

	path.insert(path.begin(), temp_path.begin(), temp_path.end());
}


//returns the average angles between vectors next_pos - curr_pos and  (threshold - curr_pos) looking ahead from index
float Autopilot::compute_angle(size_t index, _vec2 curr_pos, _vec2 next_pos, size_t threshold) {

	float angle = 0.0f;
	glm::vec3 new_direction;
	glm::vec3 temp(next_pos.x, 1, next_pos.z);
	glm::vec3 curr(curr_pos.x, 1, curr_pos.z);

	new_direction = glm::normalize(temp - curr);
	angle = atan2(new_direction.x, new_direction.z);

	if (path.size() == 0) {
		return angle;
	}

	//Average coming angles
	std::vector<_vec2>::iterator end = path.end();

	if (path.size() > (threshold + index)) {
		end = path.end() - threshold - index;
	}
	else {
		end = path.begin();
	}

	std::vector<_vec2>::iterator i = path.end() - index;
	while (i != end)
	{
		--i;

		glm::vec3 temp(i->x, 1, i->z);
		new_direction = glm::normalize(temp - curr);
		angle = (angle + atan2(new_direction.x, new_direction.z)) / 2;
	}

	return angle;
}

//check if a stop is coming up in our path
bool Autopilot::check_upcoming_stop(size_t* stop_pos) {

	std::vector<_vec2>::iterator end = path.end();

	if (path.size() == 0) {
		return false;
	}

	if (StopsMap->count(pos) == 1) {
		*stop_pos = 1;
		return true;
	}


	if (path.size() > STOP_LOOK_AHEAD) {
		end = path.end() - STOP_LOOK_AHEAD;
	}
	else {
		end = path.begin();
	}

	int pos = 0;
	std::vector<_vec2>::iterator i = path.end();
	while (i != end)
	{
		++pos;
		--i;
		if (StopsMap->count(*i) == 1) {
			*stop_pos = pos;
			return true;
		}		
	}

	return false;
}


bool Autopilot::check_upcoming_turn(size_t index, size_t* turn_pos, float * angle) {

	std::vector<_vec2>::iterator end = path.end();

	if (path.size() == 0) {
		return false;
	}

	if (path.size() > (TURN_LOOK_AHEAD + index)) {
		end = path.end() - TURN_LOOK_AHEAD - index;
	}
	else {
		end = path.begin();
	}

	std::vector<_vec2>::iterator i = path.end() - index;
	int pos = index;
	while (i != end)
	{
		--i;		
		if (check_turn_side(pos, angle)) {
			*turn_pos = pos;
			return true;
		}
		++pos;
	}
	return false;
}


//doesn't check street threshold
_vec2 Autopilot::nearest_point_in_direction(_vec2 pos, _vec2 dir) {
	_vec2 result;
	_vec2 unit = { float(m_Terrain->X_SCALAR), float(m_Terrain->Z_SCALAR) };

	if (pos == dir) {
		return pos;
	}

	if (dir.x > pos.x) {
		result.x = pos.x + unit.x; // = increment_pos(pos, 1, 0); would do the same
	}
	else if (dir.x < pos.x) {
		result.x = pos.x - unit.x;
	}
	else {
		result.x = pos.x;
	}


	if (dir.z > pos.z) {
		result.z = pos.z + unit.z;
	}
	else if (dir.z < pos.z) {
		result.z = pos.z - unit.z;
	}
	else {
		result.z = pos.z;
	}

	return result;
}


_vec2 Autopilot::increment_pos(_vec2 pos, int x, int z) {
	_vec2 unit = { float(m_Terrain->X_SCALAR), float(m_Terrain->Z_SCALAR) };

	pos.x = pos.x + float(x) * unit.x;
	pos.z = pos.z + float(z) * unit.z;

	return pos;
}

_vec2 Autopilot::scale_position(_vec2 pos) {

	_vec2 result;

	result.x = float((int(pos.x) + (m_Terrain->X_SCALAR / 2)) / m_Terrain->X_SCALAR * m_Terrain->X_SCALAR);
	result.z = float((int(pos.z) + (m_Terrain->Z_SCALAR / 2)) / m_Terrain->Z_SCALAR * m_Terrain->Z_SCALAR);

	return result;
}

_vec2 Autopilot::scale_position(glm::vec3 pos) {
	_vec2 result;

	result.x = pos.x;
	result.z = pos.z;

	return scale_position(result);
}


_vec2 Autopilot::irl_scale(_vec2 pos) {

	_vec2 irl_pos;

	irl_pos.x = float(pos.x * IRL_WIDTH / (m_Terrain->MAX_X_POS * m_Terrain->X_SCALAR));
	irl_pos.z = float(pos.z * IRL_WIDTH / (m_Terrain->MAX_X_POS * m_Terrain->X_SCALAR));

	return irl_pos;
}

float Autopilot::irl_scale(float dist) {

	return float(dist * IRL_WIDTH / (m_Terrain->MAX_X_POS * m_Terrain->X_SCALAR));
}


_vec2 Autopilot::map_scale(float dist_cm, int * pop, float* remainder) {

	/*int disp = int(*remainder + (dist_cm * (m_Terrain->MAX_X_POS * m_Terrain->X_SCALAR))/ IRL_WIDTH);
	*remainder += float(disp % m_Terrain->X_SCALAR);
	disp /= m_Terrain->X_SCALAR;*/

	float actual_disp = float(*remainder + (dist_cm * m_Terrain->MAX_X_POS / IRL_WIDTH));

	int disp = int(actual_disp);

	*remainder = (actual_disp - disp);

	std::vector<_vec2>::iterator end = path.end();

	if (disp == 0) {
		*pop = 0;
		return path.back();
	}

	if (path.size() > size_t(disp)) {
		end = path.end() - disp;
	}
	else {
		end = path.begin();
	}
	*pop = disp;

	return *end;
}


bool Autopilot::point_exists(_vec2 pos) {
	if (StreetMap->count(pos) == 1) {
		return true;
	}
	return false;
}


bool Autopilot::point_meets_threshold(_vec2 pos) {
	if (point_exists(pos)) {
		if (StreetMap->find(pos)->second < STREET_IDENTIFIER_THRESHOLD) {
			return true;
		}
	}
	return false;
}


//cmd: LEFT, RIGHT, FASTER, SLOWER
void Autopilot::SEND_ControlCommand(std::string cmd) {

	my_MSG cmd_msg;
	cmd_msg.type = "COMMAND";
	cmd_msg.message = cmd;
	m_ConnectionManager->mySend(cmd_msg);

}