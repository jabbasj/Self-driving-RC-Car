#include "all_headers.h"

#define MAX_SUBPATH_LENGTH 100
#define STREET_IDENTIFIER_THRESHOLD 7
#define STOP_LOOK_AHEAD 20

//TODO: Real-life map distance equivalance
//TODO: Break path into linear sections
//TODO: Figure out turning angles from path
//TODO: Add bluetooth communication
//TODO: Synchronize with RC car through bluetooth (from speed, time estimate where car is? or have checkpoints detected by car sensor?)
//TODO: Create seperate "driver" class for low level functions (moving/directing the car), syncs with Autopilot via bluetooth
//TODO: Implement high level FSM in driver class (first figure out what states will exist: example: waiting, straight motion, turning left, turning right, stopping, stopped, synchronizing)

class Autopilot {

public:
	Autopilot(GLRenderer* rend);
	void set_start(glm::vec3 pos);
	void set_destination(glm::vec3 dest);

	void start_autopilot();
	void stop_autopilot();
	bool check_upcoming_stop(_vec2 curr_pos, std::vector<_vec2> sub_path);
	float adjust_angle(_vec2 next_pos);

private:
	GLRenderer * m_Renderer;
	Terrain * m_Terrain;
	std::multimap<_vec2, int> * StreetMap;
	std::multimap<_vec2, int> * StopsMap;

	std::vector<std::vector<_vec2>> paths;

	_vec2 start;
	_vec2 destination;

	glm::vec3 * position;
	glm::vec3 * direction;
	bool PATH_READY;
	bool SKIP_STOP;
	bool STOP_DETECTED;

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