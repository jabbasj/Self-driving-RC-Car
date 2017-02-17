#include "all_headers.h"

#define STREET_IDENTIFIER_THRESHOLD 7
#define TURN_ANGLE_THRESHOLD 30

#define STOP_LOOK_AHEAD 20
#define ANGLE_LOOK_AHEAD 5 //where to point
#define TURN_LOOK_AHEAD 20 //considered for coming turn

#define IRL_WIDTH 254 //cm


//TODO: Real-life map distance equivalance
//TODO: Break path into linear sections?
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
	bool check_upcoming_stop(_vec2 curr_pos);
	float compute_angle(_vec2 next_pos, int threshold);
	_vec2 irl_scale(_vec2 pos);

private:
	GLRenderer * m_Renderer;
	Terrain * m_Terrain;
	std::multimap<_vec2, int> * StreetMap;
	std::multimap<_vec2, int> * StopsMap;

	std::vector<_vec2> path;

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