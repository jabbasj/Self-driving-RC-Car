#include "all_headers.h"

#define MAX_SUBPATH_LENGTH 100
#define STREET_IDENTIFIER_THRESHOLD 7

//TODO: Implement path generation (broken into linear sections) based on street identifier
//TODO: Implement high level FSM here
//TODO: Add "driver" class for low level functions (moving/directing the car)

class Autopilot {

public:
	Autopilot(GLRenderer* rend) {
		m_Renderer = rend;
		m_Terrain = &rend->m_Terrain;
		StreetMap = &m_Terrain->StreetMap;

		position = &m_Renderer->position;
		direction = &m_Renderer->direction;
		PATH_READY = false;
	}

	void set_start(glm::vec3 pos);
	void set_destination(glm::vec3 dest);

	void start_autopilot();
	void stop_autopilot();

private:
	GLRenderer * m_Renderer;
	Terrain * m_Terrain;
	std::multimap<_vec2, int> * StreetMap;

	std::vector<std::vector<_vec2>> paths;

	_vec2 start;
	_vec2 destination;

	glm::vec3 * position;
	glm::vec3 * direction;
	bool PATH_READY;

private:
	void generate_path();
	void follow_path();
	_vec2 nearest_point_in_direction(_vec2 pos, _vec2 dir);
	_vec2 scale_position(_vec2 pos);
	_vec2 scale_position(glm::vec3 pos);
	_vec2 increment_pos(_vec2 pos, int x = 0, int z = 0);
	bool point_exists(_vec2 pos);
	bool point_meets_threshold(_vec2 pos);
};