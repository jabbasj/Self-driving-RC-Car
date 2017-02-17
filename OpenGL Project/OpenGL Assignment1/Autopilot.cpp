#include "all_headers.h"

Autopilot::Autopilot(GLRenderer* rend) {
	m_Renderer = rend;
	m_Terrain = &rend->m_Terrain;
	StreetMap = &m_Terrain->StreetMap;
	StopsMap = &m_Terrain->StopsMap;

	position = &m_Renderer->position;
	direction = &m_Renderer->direction;
	PATH_READY = false;
	SKIP_STOP = false;
	STOP_DETECTED = false;
}


void Autopilot::set_start(glm::vec3 pos) {

	start = scale_position(pos);

	if (point_meets_threshold(start)) {
		std::cout << "START SET\n";
	}
	else {
		std::cout << "Invalid, try again...\n";
	}
}


void Autopilot::set_destination(glm::vec3 pos) {

	destination = scale_position(pos);

	if (point_meets_threshold(destination)) {
		std::cout << "DESTINATION SET\n";
	}
	else {
		std::cout << "Invalid, try again...\n";
	}
}


void Autopilot::start_autopilot() {

	PATH_READY = false;
	
	generate_path();

	PATH_READY = true;
}

void Autopilot::stop_autopilot() {

	if (PATH_READY) {
		std::thread fp(&Autopilot::follow_path, this);
		fp.detach();
	}
	else {
		std::cout << "Path not yet ready!\n";
	}
}

void Autopilot::follow_path() {

	std::cout << "Following path...";
	PATH_READY = false;
	SKIP_STOP = false;
	STOP_DETECTED = false;
	int counter = -1;
	float next_angle = 0.0f;
	float turn_angle = 0.0f;
	_vec2 irl_pos;
	_vec2 last_irl_pos = { 0, 0 };
	float distance = 0.0f;

	while (path.size() > 0) {
		
		_vec2 next_pos = path.back();
		path.pop_back();

		counter++;
		irl_pos = irl_scale(next_pos);
		if (last_irl_pos.x != 0 && last_irl_pos.z != 0) {
			distance += sqrt(pow(last_irl_pos.x - irl_pos.x, 2) + pow(last_irl_pos.z - irl_pos.z, 2));
		}
		
		next_angle = compute_angle(next_pos, ANGLE_LOOK_AHEAD);
		turn_angle = compute_angle(next_pos, TURN_LOOK_AHEAD);
		
		float angle_diff = (next_angle - turn_angle) * 180 / 3.14f;

		if (abs(angle_diff) > TURN_ANGLE_THRESHOLD /* degrees*/ && abs(angle_diff) < 180) {

			int index = TURN_LOOK_AHEAD;
			_vec2 temp;
			if (path.size() >= index) {
				temp = *(path.end() - TURN_LOOK_AHEAD);
			}
			else {
				temp = *path.begin();
			}

			glm::vec3 vecTemp = glm::normalize(glm::vec3(temp.x, position->y, temp.z) - *position);
			glm::vec3 vecA = *direction;

			float target_angle = vecA.x * vecTemp.z - vecA.z * vecTemp.x;
			
			if (target_angle > 0) {
				std::cout << "\nTurning right..." << angle_diff;
			}
			else {
				std::cout << "\nTurning left..." << angle_diff;
			}
		}

		m_Renderer->horizontalAngle = next_angle;
		position->x = next_pos.x;
		position->z = next_pos.z;

		Sleep(100);

		//Check for incoming stops
		if (counter % STOP_LOOK_AHEAD == 0) {
			STOP_DETECTED = check_upcoming_stop(next_pos);
		}

		if (STOP_DETECTED) {

			if (!SKIP_STOP) {
				//expecting stop
				std::cout << "\nStop detected...";
			}

			if (StopsMap->count(next_pos) == 1) {
				//stop found

				if (!SKIP_STOP) {
					std::cout << "\nSTOPPING!";
					Sleep(2000);
				}
				counter = -1;
				SKIP_STOP = !SKIP_STOP;
				STOP_DETECTED = false;
			}
		}
		last_irl_pos = irl_pos;
	}

	std::cout << "\nArrived! IRL distance travelled: " << distance << "cm\n";
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
	// This method returns vector of coordinates from target to source.
	auto gen_path = generator.findPath({ int(start.x), int(start.z) }, { int(destination.x), int(destination.z) });

	path.clear();
	_vec2 pos;
	
	for (auto& coordinate : gen_path) {
		//std::cout << coordinate.x << " " << coordinate.y << "\n";
		pos.x = float(coordinate.x); pos.z = float(coordinate.y);
		path.push_back(pos);
	}
}

float Autopilot::compute_angle(_vec2 next_pos, int threshold) {

	float angle = 0.0f;
	glm::vec3 new_direction;
	glm::vec3 temp(next_pos.x, 1, next_pos.z);

	new_direction = glm::normalize(temp - *position);
	angle = atan2(new_direction.x, new_direction.z);

	if (path.size() == 0) {
		return angle;
	}

	//Average coming angles
	std::vector<_vec2>::iterator end = path.end();

	if (path.size() > threshold) {
		end = path.end() - threshold;
	}
	else {
		end = path.begin();
	}

	std::vector<_vec2>::iterator i = path.end();
	while (i != end)
	{
		--i;

		glm::vec3 temp(i->x, 1, i->z);
		new_direction = glm::normalize(temp - *position);
		angle = (angle + atan2(new_direction.x, new_direction.z)) / 2;
	}

	return angle;
}

//check if a stop is coming up in our path
bool Autopilot::check_upcoming_stop(_vec2 curr_pos) {

	std::vector<_vec2>::iterator end = path.end();

	if (path.size() == 0) {
		return false;
	}

	if (StopsMap->count(curr_pos) == 1) {
		return true;
	}


	if (path.size() > STOP_LOOK_AHEAD) {
		end = path.end() - STOP_LOOK_AHEAD;
	}
	else {
		end = path.begin();
	}

	std::vector<_vec2>::iterator i = path.end();
	while (i != end)
	{
		--i;
		if (StopsMap->count(*i) == 1) {
			return true;
		}		
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

	irl_pos.x = pos.x * IRL_WIDTH / (m_Terrain->MAX_X_POS * m_Terrain->X_SCALAR);
	irl_pos.z = pos.z * IRL_WIDTH / (m_Terrain->MAX_X_POS * m_Terrain->X_SCALAR);

	return irl_pos;
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