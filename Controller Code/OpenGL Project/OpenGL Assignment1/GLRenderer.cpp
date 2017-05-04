#include "all_headers.h"

GLRenderer::GLRenderer(GLFWwindow * _win){ win = _win; }

// Loads the shaders and programs, initializes opengl params
// Mesh and Terrain Setup themselves
void GLRenderer::PrepareScene()
{
	// Grey background
	glClearColor(0.170f, 0.170f, 0.200f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES); // Default wireframe

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable backface culling
	glEnable(GL_CULL_FACE);

	/*vvvvvvvvvvvvvvvvvvvv Loading shaders vvvvvvvvvvvvvvvvv*/

	// All purpose shaders
	m_pProgram = new GLProgram();
	m_pVertSh = new GLShader(GL_VERTEX_SHADER);
	m_pFragSh = new GLShader(GL_FRAGMENT_SHADER);

	int status = 0;

	status += (int) m_pVertSh->Load("lit.vert");
	status += (int) m_pFragSh->Load("lit.frag");

	status += (int) m_pVertSh->Compile();
	status += (int) m_pFragSh->Compile();

	m_pProgram->AttachShader(m_pVertSh);
	m_pProgram->AttachShader(m_pFragSh);

	status += (int) m_pProgram->Link();

	// Get a handle for the "MVP" uniform
	MatrixID = glGetUniformLocation(m_pProgram->GetID(), "MVP");
	M_MatrixID = glGetUniformLocation(m_pProgram->GetID(), "M");
	V_MatrixID = glGetUniformLocation(m_pProgram->GetID(), "V");


	if (status != 5 ) {
		std::cout << "Error during loading/compiling/linking of shaders!!\n";
	}

	m_pProgram->Use();

	SetData();
}


void GLRenderer::PrepareLights() {

	/*		  type_light_index		*/
	LightInfo point_light_0;

	point_light_0.Type = 0; //Point
	point_light_0.LightPosition_worldspace = glm::vec3(2000, 2000, 2000);
	point_light_0.LightColor = glm::vec3(1, 0, 0);
	point_light_0.LightPower = 1000000.0f;

	point_light_0.Type_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[0].Type");
	glUniform1i(point_light_0.Type_ID, point_light_0.Type);

	point_light_0.LightPosition_worldspace_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[0].LightPosition_worldspace");
	glUniform3fv(point_light_0.LightPosition_worldspace_ID, 1, &point_light_0.LightPosition_worldspace[0]);

	point_light_0.LightColor_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[0].LightColor");
	glUniform3fv(point_light_0.LightColor_ID, 1, &point_light_0.LightColor[0]);

	point_light_0.LightPower_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[0].LightPower");
	glUniform1f(point_light_0.LightPower_ID, point_light_0.LightPower);



	LightInfo point_light_1;

	point_light_1.Type = 0; //Point
	point_light_1.LightPosition_worldspace = glm::vec3(1000, 2000, 1000);
	point_light_1.LightColor = glm::vec3(1, 0, 0);
	point_light_1.LightPower = 1000000.0f;

	point_light_1.Type_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[1].Type");
	glUniform1i(point_light_1.Type_ID, point_light_1.Type);

	point_light_1.LightPosition_worldspace_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[1].LightPosition_worldspace");
	glUniform3fv(point_light_1.LightPosition_worldspace_ID, 1, &point_light_1.LightPosition_worldspace[0]);

	point_light_1.LightColor_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[1].LightColor");
	glUniform3fv(point_light_1.LightColor_ID, 1, &point_light_1.LightColor[0]);

	point_light_1.LightPower_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[1].LightPower");
	glUniform1f(point_light_1.LightPower_ID, point_light_1.LightPower);


	/*
	LightInfo spot_light_2;

	spot_light_2.Type = 1; // Spot light
	spot_light_2.LightPosition_worldspace = position;
	spot_light_2.LightPosition_worldspace.y;
	spot_light_2.SpotLightDirection_worldspace = direction;
	spot_light_2.LightColor = glm::vec3(0.355f, 0.247f, 0.141f);
	spot_light_2.LightPower = 50000.0f;
	spot_light_2.CutOff = cos(glm::radians(12.5f));
	spot_light_2.OuterCutOff = cos(glm::radians(15.0f));
	spot_light_2.dynamic = true;

	spot_light_2.Type_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[2].Type");
	glUniform1i(spot_light_2.Type_ID, spot_light_2.Type);

	spot_light_2.LightPosition_worldspace_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[2].LightPosition_worldspace");
	glUniform3fv(spot_light_2.LightPosition_worldspace_ID, 1, &spot_light_2.LightPosition_worldspace[0]);

	spot_light_2.SpotLightDirection_worldspace_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[2].SpotLightDirection_worldspace");
	glUniform3fv(spot_light_2.SpotLightDirection_worldspace_ID, 1, &spot_light_2.SpotLightDirection_worldspace[0]);

	spot_light_2.LightColor_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[2].LightColor");
	glUniform3fv(spot_light_2.LightColor_ID, 1, &spot_light_2.LightColor[0]);

	spot_light_2.LightPower_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[2].LightPower");
	glUniform1f(spot_light_2.LightPower_ID, spot_light_2.LightPower);

	spot_light_2.CutOff_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[2].CutOff");
	glUniform1f(spot_light_2.CutOff_ID, spot_light_2.CutOff);

	spot_light_2.OuterCutOff_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[2].OuterCutOff");
	glUniform1f(spot_light_2.OuterCutOff_ID, spot_light_2.OuterCutOff);
	*/

	m_Lights.push_back(point_light_0);
	m_Lights.push_back(point_light_1);
	//m_Lights.push_back(spot_light_2);
}


void GLRenderer::LoadBankModels() {

	ReadModelMatrices();

	bool already_loaded = false;

	for (size_t i = 0; i < model_data.size(); i++) {

		already_loaded = false;

		for (size_t j = 0; j < m_ModelsBank.size(); j++) {

			if (m_ModelsBank[j]->name == model_data[i].name) {
				already_loaded = true;
			}
		}

		if (!already_loaded) {

			m_ModelsBank.push_back(new Model(&model_data[i].path[0], model_data[i].name, model_data[i].centered));

			// Hacky, does not support rotations
			m_ModelsBank.back()->scale = glm::vec3(model_data[i].transformation[0].x, model_data[i].transformation[1].y, model_data[i].transformation[2].z);
			//m_ModelsBank.back()->scale *= 0.5f;

		}

	}
}


void GLRenderer::RenderModel(std::string name, bool transform) {

	for (size_t i = 0; i < m_ModelsBank.size(); i++) {
		if (name == m_ModelsBank[i]->name) {
			CopyModel(m_ModelsBank[i]);

			if (transform) {
				m_Models.back()->Transform(m_ModelsBank[i]->meshes[0].ModelMat);
			}
			return;
		}
	}

}


void GLRenderer::ScatterModels() {

	int copied_model_index = 0;

	for (size_t i = 0; i < model_data.size(); i++) {

		for (size_t j = 0; j < m_ModelsBank.size(); j++) {
			if (model_data[i].name == m_ModelsBank[j]->name) {
				copied_model_index = j;
				break;
			}
		}

		CopyModel(m_ModelsBank[copied_model_index]);
		m_Models.back()->Transform(model_data[i].transformation);

		Sleep(25);
	}
}

// Fills the ModelData array and loads all the models defined in "Object Matrices.txt"
// Add custom models here, or in the above file
void GLRenderer::SetData()
{
	PrepareLights();

	LoadBankModels();

	//RequestUserInput();
	m_Terrain.SetupTerrain(); // Prepare Terrain

	// Skybox model
	m_Models.push_back(new Model("./models/skybox/skybox.obj", "sky"));
	skybox = m_Models.back();	
	skybox->Scale(glm::vec3(500, 150, 500));
	skybox->AbsoluteTranslate(glm::vec3(0, 10, 0));

	// First person model
	m_Models.push_back(new Model("./models/Mars_Explorer/Mars_Explorer.obj", "first_person"));	
	first_person = m_Models.back();
	m_Models.back()->scale = glm::vec3(0.15, 0.15, 0.15);
	

	srand(size_t(time(NULL)));	
	ScatterModels();
}

//TEST STREET MAP DETECTION
void GLRenderer::move() {
	// Static -> Called only first time
	static double lastTime = glfwGetTime();

	// Time since last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	if (glfwGetKey(win, GLFW_KEY_Q) != GLFW_PRESS){
		lastTime = currentTime;
		return;
	}

	glm::vec3 currPosition = position + 75.0f * direction;
	_vec2 pos;
	pos.x = float((int(currPosition.x) + (m_Terrain.X_SCALAR / 2)) / m_Terrain.X_SCALAR * m_Terrain.X_SCALAR);
	pos.z = float((int(currPosition.z) + (m_Terrain.Z_SCALAR / 2)) / m_Terrain.Z_SCALAR * m_Terrain.Z_SCALAR);

	int street_identifier = 0;
	
	if (m_Terrain.StreetMap.count(pos) == 1) {

		street_identifier = m_Terrain.StreetMap.find(pos)->second;
		std::cout << "STREET_ID: " << street_identifier << std::endl;
		std::cout << position.x << ", " << position.y << ", " << position.z << "\n";

		if (street_identifier <= 5) {
			position += direction * deltaTime * speed;
		}
	}

	lastTime = currentTime;
}


void GLRenderer::HandleAutopilotInputs() {

	if (glfwGetKey(win, GLFW_KEY_F1) == GLFW_PRESS ||
		glfwGetKey(win, GLFW_KEY_F2) == GLFW_PRESS ||
		glfwGetKey(win, GLFW_KEY_F3) == GLFW_PRESS ||
		glfwGetKey(win, GLFW_KEY_F4) == GLFW_PRESS ||
		glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS ||
		glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS ||
		glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS ||
		glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS ||
		glfwGetKey(win, GLFW_KEY_BACKSPACE) == GLFW_PRESS
		)
	{

		static double lastTime = 0;

		// Time since last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		glm::vec3 intersection;
		float intersect_dist = 100.0f;
		float temp;

		if (deltaTime > 0.3) {

			if (glfwGetKey(win, GLFW_KEY_F1) == GLFW_PRESS){
				
				if (glm::intersectRayPlane(position, direction, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), intersect_dist)) {

					temp = (glm::dot(glm::vec3(0, 1, 0), (glm::vec3(0, 0, 0) - position))) / glm::dot(glm::vec3(0, 1, 0), direction);
					intersection = position + (direction * temp);
					m_Autopilot->set_start(intersection);
					last_pos = intersection;
				}
			}

			if (glfwGetKey(win, GLFW_KEY_F2) == GLFW_PRESS) {

				if (glm::intersectRayPlane(position, direction, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), intersect_dist)) {

					temp = (glm::dot(glm::vec3(0, 1, 0), (glm::vec3(0, 0, 0) - position))) / glm::dot(glm::vec3(0, 1, 0), direction);
					intersection = position + (direction * temp);

					m_Autopilot->set_destination(intersection);
				}
			}

			if (glfwGetKey(win, GLFW_KEY_F3) == GLFW_PRESS) {
				m_Autopilot->start_autopilot();
			}

			if (glfwGetKey(win, GLFW_KEY_F4) == GLFW_PRESS) {
				m_Autopilot->stop_autopilot();
			}


			if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				m_Autopilot->SEND_ControlCommand("RIGHT");
			}

			if (glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS) {
				m_Autopilot->SEND_ControlCommand("LEFT");
			}

			if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS) {
				m_Autopilot->SEND_ControlCommand("FASTER");
			}

			if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS) {
				m_Autopilot->SEND_ControlCommand("SLOWER");
			}

			if (glfwGetKey(win, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
				m_Autopilot->SEND_ControlCommand("STOP");
				m_Autopilot->SPEAK("RESET");
			}


			lastTime = currentTime;
		}
	}		
}

// Main part of game loop.
// Meshes & Terrain draws themselves
void GLRenderer::DrawScene()
{
	if (!fix_to_ground) {
		std::async(&GLRenderer::GroundDetection, this).get();
	}
	//std::async(&GLRenderer::CollisionDetection, this);	// In Parallel
	std::async(&GLRenderer::RayTracing, this);			// In Parallel

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (glfwWindowShouldClose(win)) return;

	move();

	UpdateMatricesFromInputs();		// Update camera position and view / projection matrices
	HandleModelManipulation();
	HandleAutopilotInputs();

	// Draw Terrain
	glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &glm::mat4()[0][0]);
	glUniformMatrix4fv(V_MatrixID, 1, GL_FALSE, &View[0][0]);
	MVP = Projection * View;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	m_Terrain.Draw(win);

	// Draw Models
	size_t models_count = m_Models.size();
	for (size_t i = 0; i < models_count; i++) {

		if (!m_Models[i]->ready) continue;

		if (m_Models[i]->name == "sky") {
			glDisable(GL_CULL_FACE);
		}
		size_t mesh_count = m_Models[i]->meshes.size();
		for (size_t j = 0; j < mesh_count; j++) {

			if (m_Models[i]->meshes[j].ready) {
				glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &m_Models[i]->meshes[j].ModelMat[0][0]);

				glUniformMatrix4fv(V_MatrixID, 1, GL_FALSE, &View[0][0]);

				MVP = Projection * View * m_Models[i]->meshes[j].ModelMat;
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			}
			m_Models[i]->meshes[j].Draw(win);
		}

		if (m_Models[i]->name == "sky") {
			glEnable(GL_CULL_FACE);
		}
	}

	// Swap buffers
	glfwSwapBuffers(win);

	HandleSpawning();
}


// Basic bounding-box detection
void GLRenderer::GroundDetection() {
	glm::vec3 currPosition = position + 25.0f * direction;
	currPosition = currPosition + 20.0f * -up;

	std::vector<_vec2> near_intersections;

	_vec2 pos;
	pos.x = std::floor(currPosition.x / m_Terrain.X_SCALAR) * m_Terrain.X_SCALAR;
	pos.z = std::floor(currPosition.z / m_Terrain.Z_SCALAR) * m_Terrain.Z_SCALAR;
	near_intersections.push_back(pos);

	pos.x += m_Terrain.X_SCALAR;
	near_intersections.push_back(pos);

	pos.z += m_Terrain.Z_SCALAR;
	near_intersections.push_back(pos);

	pos.x -= m_Terrain.X_SCALAR;
	near_intersections.push_back(pos);

	pos.x -= m_Terrain.X_SCALAR;
	near_intersections.push_back(pos);

	pos.z -= m_Terrain.Z_SCALAR;
	near_intersections.push_back(pos);


	Vertex intersection;
	glm::vec3 terrain_pos;

	for (size_t i = 0; i < near_intersections.size(); i++) {
		pos = near_intersections[i];

		if (m_Terrain.DepthMap.count(pos) > 0) {

			intersection = m_Terrain.DepthMap.find(pos)->second;
			terrain_pos = intersection.Position;

			if (currPosition.y < terrain_pos.y || glm::distance(terrain_pos, currPosition) < 10) {

				glm::vec3 push_back = intersection.Normal * speed / 5.0f;
				position += push_back;

				// Translate skybox
				skybox->AbsoluteTranslate(glm::vec3(push_back.x / skybox->scale.x, 0, push_back.z / skybox->scale.z));

				return;
			}
		}

	}
	return;
}


// Very basic sphere-based ray intersection
void GLRenderer::RayTracing() {
	bool intersected = false;
	glm::vec3 intersection_position;
	glm::vec3 intersection_normal;
	float dist = 0.0f;

	size_t model_count = m_Models.size();
	for (size_t j = 0; j < model_count; j++) {

		if (glfwWindowShouldClose(win)) { return; }
		if (m_Models[j]->name == "first_person") continue;
		if (m_Models[j]->name == "sky") continue;
		if (ignore_roads && (m_Models[j]->name == "road1" || m_Models[j]->name == "road3")) continue;
		if (!m_Models[j]->ready) { continue; }

		size_t mesh_count = m_Models[j]->meshes.size();
		for (size_t k = 0; k < mesh_count; k++) {
			Mesh * mesh = &m_Models[j]->meshes[k];

			glm::vec3 mesh_pos(mesh->ModelMat * mesh->centroid);

			if (glm::distance(position, mesh_pos) > 7500) continue;

			glm::vec3 max(mesh->ModelMat * mesh->max);
			max -= mesh_pos;
			float radius = sqrt(pow(max.x, 2) + pow(max.y, 2) + pow(max.z, 2));
			glm::vec3 pos;
			glm::vec3 norm;
			float new_dist;

			if (glm::intersectRaySphere(position, direction, mesh_pos, radius, pos, norm)) {
				new_dist = sqrt(pow(pos.x - position.x, 2) + pow(pos.y - position.y, 2) + pow(pos.z - position.z, 2));

				if (dist == 0.0f || new_dist < dist) {
					// this one is closer than old one
					dist = new_dist;
					intersection_position = pos;
					intersected = true;
					selected = m_Models[j];
				}
			}
			
		}
	}

#ifdef PRINT_COLLISIONS_AND_RAYS
	system("cls");
#endif
	if (intersected) {
#ifdef PRINT_COLLISIONS_AND_RAYS
		std::cout << "RAY CROSSED: " << selected->name << "\n";
#endif
		
	}
	else {
		selected = &null_model;
	}

	return;
}



// Very basic box-based
bool GLRenderer::CollisionDetection() {
	glm::vec3 currPosition = position + 75.0f * direction;
	if (!fix_to_ground) {
		currPosition = currPosition + 20.0f * -up;
	}
	bool hit = false;

	size_t model_count = m_Models.size();
	for (size_t j = 0; j < model_count; j++) {

		if (glfwWindowShouldClose(win)) { return false; }
		if (m_Models[j]->name == "first_person") continue;
		if (m_Models[j]->name == "sky") continue;
		if (m_Models[j]->name == "road1" || m_Models[j]->name == "road3") continue;
		if (!m_Models[j]->ready) { continue; }

		size_t mesh_count = m_Models[j]->meshes.size();
		for (size_t k = 0; k < mesh_count; k++) {
			Mesh * mesh = &m_Models[j]->meshes[k];

			glm::vec3 mesh_pos(mesh->ModelMat * mesh->centroid);

			if (glm::distance(position, mesh_pos) > 500) continue;

			glm::vec3 max(mesh->ModelMat * mesh->max);
			glm::vec3 min(mesh->ModelMat * mesh->min);

			if (currPosition.x <= max.x && currPosition.x >= min.x || currPosition.x >= max.x && currPosition.x <= min.x) {
				if (currPosition.y <= max.y && currPosition.y >= min.y || currPosition.y >= max.y && currPosition.y <= min.y) {
					if (currPosition.z <= max.z && currPosition.z >= min.z || currPosition.z >= max.z && currPosition.z <= min.z) {
#ifdef PRINT_COLLISIONS_AND_RAYS
						system("cls");
						std::cout << "PENETRATED: " << mesh->parent->name << "\n";	
#endif

						glm::vec3 push_back = currPosition - mesh_pos - direction;

						push_back = glm::normalize(push_back);
						push_back *= speed / 10;

						position += push_back;

						// Translate skybox
						skybox->AbsoluteTranslate(glm::vec3(push_back.x / skybox->scale.x, 0, push_back.z / skybox->scale.z));
						
						hit = true;
					}
				}
			}
		}
	}
	return hit;
}


void GLRenderer::Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}


void GLRenderer::HandleModelManipulation() {

	OutputModelMatrices();

	if (selected->name != "sky") {
		if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){

			if (selected->name == "road1" || selected->name == "road3") {
				glm::vec3 dir(direction * 2.0f);
				dir.y = 0;
				ModelAbsoluteTranslate(dir, selected);
			}
			else {
				ModelAbsoluteTranslate(direction * 2.0f, selected);
			}
		}

		if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS){

			if (selected->name == "road1" || selected->name == "road3") {
				glm::vec3 dir(-direction * 2.0f);
				dir.y = 0;
				ModelAbsoluteTranslate(dir, selected);
			}
			else {
				ModelAbsoluteTranslate(-direction * 2.0f, selected);
			}
		}

		if (glfwGetKey(win, GLFW_KEY_KP_ADD) == GLFW_PRESS){
			ModelScale(glm::vec3(1.001f, 1.001f, 1.001f), selected);
		}

		if (glfwGetKey(win, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS){
			ModelScale(glm::vec3(0.999f, 0.999f, 0.999f), selected);
		}

		/*
		if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS){
			ModelRotateSelf(0.01f, selected);
		}

		if (glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS){
			ModelRotateSelf(-0.01f, selected);
		}*/


		if (glfwGetKey(win, GLFW_KEY_G) == GLFW_PRESS) {
			// Static -> Called only first time
			static double last_time_toggled_ground = 3;		
			static float last_horizontalAngle = horizontalAngle; // Initial horizontal angle : toward -Z		(angle around Y)
			static float last_verticalAngle = verticalAngle; // Initial vertical angle : none			(angle around X)		

			// Time since last frame
			double currentTime = glfwGetTime();
			float deltaTime = float(currentTime - last_time_toggled_ground);

			if (deltaTime > 1) {
				first_person->ready = !first_person->ready;
				fix_to_ground = !fix_to_ground;

				if (fix_to_ground) {
					position = last_pos;
					horizontalAngle = last_horizontalAngle;
					verticalAngle = last_verticalAngle;
				}
				else {					
					last_pos = position;
					last_horizontalAngle = horizontalAngle;
					last_verticalAngle = verticalAngle;
					verticalAngle = -3.14f / 2;

					position.y = float(m_Terrain.MAX_X_POS * m_Terrain.X_SCALAR);
					position.x = float(m_Terrain.MAX_X_POS / 2 * m_Terrain.X_SCALAR);
					position.z = float(m_Terrain.MAX_Z_POS / 2 * m_Terrain.Z_SCALAR);
				}
			}
			last_time_toggled_ground = currentTime;
		}

		if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS) {
			// Static -> Called only first time
			static double last_time_toggled_roads = 3;

			// Time since last frame
			double currentTime = glfwGetTime();
			float deltaTime = float(currentTime - last_time_toggled_roads);

			if (deltaTime > 1) {
				ignore_roads = !ignore_roads;
			}
			last_time_toggled_roads = currentTime;
		}

	}
}


void GLRenderer::HandleSpawning() {

	if (glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS) {
		// Static -> Called only first time
		static double last_time_spawned = 3;

		// Time since last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - last_time_spawned);

		if (deltaTime > 1) {
			CopyModelAtMyLocation(selected);
		}
		last_time_spawned = currentTime;
	}

	if (glfwGetKey(win, GLFW_KEY_DELETE) == GLFW_PRESS) {
		// Static -> Called only first time
		static double last_time_deleted = 3;

		// Time since last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - last_time_deleted);

		if (deltaTime > 1) {
			selected->ready = false;
		}
		last_time_deleted = currentTime;
	}
}


void GLRenderer::CopyModelAtMyLocation(Model* copy_this) {
	m_Models.push_back(new Model(copy_this));

	
	if (m_Models.back()->name == "road1" || m_Models.back()->name == "road3") {
		glm::vec3 dir(position);
		dir.y = m_Terrain.DepthMap.begin()->second.Position.y + 5;
		m_Models.back()->AbsoluteTranslate(dir);
	}
	else {
		m_Models.back()->AbsoluteTranslate(position);
	}
	m_Models.back()->Scale(copy_this->scale);
}


Model* GLRenderer::CopyModel(Model* copy_this) {
	m_Models.push_back(new Model(copy_this));

	return m_Models.back();
}

// Source: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
void GLRenderer::UpdateMatricesFromInputs(){
	int width, height;
	glfwGetWindowSize(win, &width, &height);

	// Static -> Called only first time
	static double lastTime = glfwGetTime();

	// Time since last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Mouse pos
	double xpos, ypos;
	glfwGetCursorPos(win, &xpos, &ypos);

	// Convert movement into yaw / pitch
	horizontalAngle += mouseSpeed * float(width / 2 - xpos);
	verticalAngle += mouseSpeed * float(height / 2 - ypos);

	// Direction (Spherical to Cartesian), trickiest part...
	direction = glm::vec3(
					sin(horizontalAngle) * cos(verticalAngle),
					sin(verticalAngle),
					cos(verticalAngle) * cos(horizontalAngle)
					);



	//std::cout << "test horizontalAngle: " << atan2(direction.x, direction.z) << " rad\n";


	// TODO: added for debugging purposes, remove this when submit
	//printf("direc: %f, %f, %f \n", direction.x, direction.y, direction.z);
	//printf("pos: %f, %f, %f \n", position.x, position.y, position.z);

	// Right vector
	right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector
	up = glm::cross(right, direction);
	glm::vec3 old_position = position;

	// Movement with Mouse
	// Move forward
	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}

	// Strafe right
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}
	// Strafe up
	if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS){
		position += up * deltaTime * speed;
	}

	if (fix_to_ground) {
		position.y = m_Terrain.DepthMap.begin()->second.Position.y + 25;
	}

	// Resize window with Page Up / Down
	if (glfwGetKey(win, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS){
		glfwSetWindowSize(win, width - width/100, height - height/100);
		Reshape(width - width / 100, height - height / 100);
	}

	if (glfwGetKey(win, GLFW_KEY_PAGE_UP) == GLFW_PRESS){
		glfwSetWindowSize(win, width + width / 100, height + height / 100);
		Reshape(width + width / 100, height + height / 100);
	}

	// Translate skybox
	glm::vec3 delta = position - old_position;
	skybox->AbsoluteTranslate(glm::vec3(delta.x / skybox->scale.x, 0, delta.z / skybox->scale.z));

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 11000.0f);

	// Camera matrix
	View = glm::lookAt(position, position + direction, up);	

	// Handle first person models
	glm::vec3 scale = first_person->scale;
	ModelReset(first_person);
	ModelRelativeTranslate(position, first_person);
	ModelRotate(horizontalAngle - 3.14f * 2, up, first_person);
	ModelRotate(verticalAngle, right, first_person);
	ModelRelativeTranslate(glm::vec3(3, -20, 35), first_person);
	ModelRotate(0.1f, right, first_person);
	ModelRotate(0.05f, up, first_person);
	ModelScale(scale, first_person);

	// Reset mouse position for next frame
	glfwSetCursorPos(win, width / 2, height / 2);

	/* Update spotlight pos/dir
	m_Lights[2].LightPosition_worldspace = position;
	m_Lights[2].LightPosition_worldspace.y;
	m_Lights[2].SpotLightDirection_worldspace = direction;

	glUniform3fv(m_Lights[2].LightPosition_worldspace_ID, 1, &m_Lights[2].LightPosition_worldspace[0]);
	glUniform3fv(m_Lights[2].SpotLightDirection_worldspace_ID, 1, &m_Lights[2].SpotLightDirection_worldspace[0]);

	if (glfwGetKey(win, GLFW_KEY_L) == GLFW_PRESS){

		if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			m_Lights[2].LightPower += 100.0f;
		}
		else {
			if (m_Lights[2].LightPower >= 100.0f) {
				m_Lights[2].LightPower -= 100.0f;
			}
		}


		m_Lights[2].LightPower_ID = glGetUniformLocation(m_pProgram->GetID(), "Lights[2].LightPower");
		glUniform1f(m_Lights[2].LightPower_ID, m_Lights[2].LightPower);
	}
	*/
	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}


// Detach shader programs, delete the pointers
// Sleep to let the asynchronous functions finish
void GLRenderer::DestroyScene()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_pProgram->DetachShader(m_pVertSh);
	m_pProgram->DetachShader(m_pFragSh);

	Sleep(5000);

	for (size_t i = 0; i < m_Models.size(); i++) {
		delete m_Models[i];
	}

	for (size_t i = 0; i < m_ModelsBank.size(); i++) {
		delete m_ModelsBank[i];
	}

	delete m_pProgram;
	delete m_pVertSh;
	delete m_pFragSh;
}


// Based on specific model
void GLRenderer::ModelCenter(Model* update_this) {
	update_this->Center();
}

void GLRenderer::ModelAbsoluteTranslate(glm::vec3 change, Model* update_this) {
	update_this->AbsoluteTranslate(change);
}

void GLRenderer::ModelRelativeTranslate(glm::vec3 change, Model* update_this) {
	update_this->RelativeTranslate(change);
}

void GLRenderer::ModelScale(glm::vec3 change, Model* update_this) {
	update_this->Scale(change);
}

void GLRenderer::ModelRotate(float angle, glm::vec3 vector, Model* update_this){
	update_this->Rotate(angle, vector);
}

void GLRenderer::ModelReset(Model* update_this){
	update_this->Reset();
}

void GLRenderer::ModelTransform(glm::mat4 tm, Model* update_this) {
	update_this->Transform(tm);
}

void GLRenderer::ModelRotateSelf(float angle, Model* update_this) {
	update_this->RotateSelf(angle);
}

// Based on name
void GLRenderer::ModelCenter(std::string name) {
	for (size_t i = 0; i < m_Models.size(); i++) {
		if (m_Models[i]->name == name) {
			m_Models[i]->Center();
		}
	}
}

void GLRenderer::ModelTranslate(glm::vec3 change, std::string name) {
	for (size_t i = 0; i < m_Models.size(); i++) {
		if (m_Models[i]->name == name) {
			m_Models[i]->AbsoluteTranslate(change);
		}
	}
}

void GLRenderer::ModelScale(glm::vec3 change, std::string name) {
	for (size_t i = 0; i < m_Models.size(); i++) {
		if (m_Models[i]->name == name) {
			m_Models[i]->Scale(change);
		}
	}
}

void GLRenderer::ModelRotate(float angle, glm::vec3 vector, std::string name){
	for (size_t i = 0; i < m_Models.size(); i++) {
		if (m_Models[i]->name == name) {
			m_Models[i]->Rotate(angle * 1, vector);
		}
	}
}

void GLRenderer::ModelReset(std::string name) {
	for (size_t i = 0; i < m_Models.size(); i++) {
		if (m_Models[i]->name == name) {
			m_Models[i]->Reset();
		}
	}
}

void GLRenderer::ModelTransform(glm::mat4 tm, std::string name) {
	for (size_t i = 0; i < m_Models.size(); i++) {
		if (m_Models[i]->name == name) {
			m_Models[i]->Transform(tm);
		}
	}
}

// Writes all the existing model's Current Transformation Matrix into "Object Matrices.txt"
void GLRenderer::OutputModelMatrices() {
	if (glfwGetKey(win, GLFW_KEY_O) == GLFW_PRESS) {

		std::ofstream File("Object Matrices.txt");

		for (size_t i = 0; i < m_Models.size(); i++) {	
			Model* model = m_Models[i];

			if (model->name == "first_person") continue;
			if (model->name == "sky") continue;
			if (!model->ready) continue;


			if (model->meshes.size() > 0) {		

				glm::mat4 matrix = model->meshes[0].ModelMat;
				std::string is_centered = "false";
				if (model->centered) is_centered = "true";
				File << model->name << "\t[" << model->path << "]\t[" << model->mID << "]\t{" << is_centered << "}\n" << glm::to_string(matrix) << "\n\n";
			}
		}
	}
}

// Read "Object Matrices.txt" and fills the ModelData array, which holds transformation for a model
void GLRenderer::ReadModelMatrices() {

	std::ifstream File("Object Matrices.txt");
	std::string Data;

	glm::mat4 transformation;
	ModelData model_info;

	if (!File.is_open()) return;

	while (!File.eof()) {
		// Get name, path, index and centered
		getline(File, Data);
		if (Data.empty()) continue;

		int sep0 = Data.find_first_of('\t');

		std::string name = Data.substr(0, sep0);

		sep0 = Data.find_first_of('[');
		int sep1 = Data.find_first_of(']');
		std::string path = Data.substr(sep0 + 1, sep1 - sep0 - 1);

		sep0 = Data.rfind('[');
		sep1 = Data.rfind(']');
		int id = std::stoi(Data.substr(sep0 + 1, sep1 - sep0 - 1));

		sep0 = Data.rfind('{');
		sep1 = Data.rfind('}');

		std::string cent = Data.substr(sep0 + 1, sep1 - sep0 - 1);
		bool is_centered = true;
		if (cent == "false") is_centered = false;

		// Get transformation matrix
		getline(File, Data);

		float temp_float;
		sep0 = Data.find_first_of("((");
		sep1 = Data.rfind("))");

		Data = Data.substr(sep0 + 1, sep1 - sep0 - 1);

		glm::vec4 temp_vec;
		std::string temp_string;

		for (size_t i = 0; i < 4; i++) {

			sep0 = Data.find_first_of('(');
			sep1 = Data.find_first_of(',');

			temp_string = Data.substr(sep0 + 1, sep1 - sep0 - 1);
			temp_float = std::stof(temp_string);
			temp_vec.x = temp_float;

			Data = Data.substr(sep1 + 2, -1);

			sep0 = 0;
			sep1 = Data.find_first_of(',');
			temp_string = Data.substr(sep0, sep1 - sep0 - 1);
			temp_float = std::stof(temp_string);
			temp_vec.y = temp_float;

			Data = Data.substr(sep1 + 2, -1);

			sep0 = 0;
			sep1 = Data.find_first_of(',');
			temp_string = Data.substr(sep0, sep1 - sep0 - 1);
			temp_float = std::stof(temp_string);
			temp_vec.z = temp_float;

			Data = Data.substr(sep1 + 2, -1);

			sep0 = 0;
			sep1 = Data.find_first_of(')');
			temp_string = Data.substr(sep0, sep1 - sep0 - 1);
			temp_float = std::stof(temp_string);
			temp_vec.w = temp_float;

			if (i < 3) {
				Data = Data.substr(sep1 + 2, -1);
			}

			transformation[i] = temp_vec;
		}

		// initialize model data struct
		model_info.id = id;
		model_info.path = path;
		model_info.name = name;
		model_info.transformation = transformation;
		model_info.centered = is_centered;
		model_info.instanced = false;

		model_data.push_back(model_info);

		NUM_OF_MODELS++;
	}
}


void GLRenderer::RequestUserInput() {

	int num = 0;
	std::string input = "";

	while (num > 50 || num <= 0) {
		std::cout << "Enter the number of models to spawn randomly: \n";
		try {
			std::cin >> input;
			num = std::stoi(input);

			if (num > 50 || num <= 0) {
				std::cout << "Number must be between 1 and 50!\n";
			}
		}
		catch (...) {
			std::cout << "Wrong input!\n";
			num = 0;
			input = "";
		}
	}

	NUM_OF_MODELS = num;
}