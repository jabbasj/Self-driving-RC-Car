#include "all_headers.h"

#define STREET_IDENTIFIER_THRESHOLD 7
#define TURN_ANGLE_THRESHOLD 30

#define STOP_LOOK_AHEAD 30
#define ANGLE_LOOK_AHEAD 8 //where to point
#define TURN_LOOK_AHEAD 37 //considered for coming turn

#define IRL_WIDTH 302.56 //cm


class Autopilot {

public:
	Autopilot(GLRenderer* rend);
	~Autopilot();
	void set_start(glm::vec3 pos);
	void set_destination(glm::vec3 dest);
	void SEND_ControlCommand(std::string cmd);

	void start_autopilot();
	void stop_autopilot();
	void SPEAK(std::string string, bool force = false);

private:
	gds::CTextSpeaker m_Speaker;
	GLRenderer * m_Renderer;
	Terrain * m_Terrain;
	libxbee::XBee * xbee;
	ConnectionManager * m_ConnectionManager;
	std::multimap<_vec2, int> * StreetMap;
	std::multimap<_vec2, int> * StopsMap;

	std::vector<_vec2> path;

	_vec2 start;
	_vec2 pos;
	_vec2 destination;

	glm::vec3 * position;
	glm::vec3 * direction;
	bool PATH_READY;
	bool FOLLOWING_PATH;
	bool CONNECTION_ACTIVE;

	std::thread fp;

	void SEND_FinishMessage();
	void SEND_StartMessage();
	void SEND_StopSignMessage(int dist);
	void SEND_TurnMessage(int angle, int dist);

	void HANDLE_ReceivedMessage(my_MSG* msg, float * disp);



private:

	void update_renderer_location(float new_angle, _vec2 new_pos);
	void generate_path();
	void follow_path();
	_vec2 nearest_point_in_direction(_vec2 pos, _vec2 dir);
	_vec2 scale_position(_vec2 pos);
	_vec2 scale_position(glm::vec3 pos);
	_vec2 increment_pos(_vec2 pos, int x = 0, int z = 0);
	bool point_exists(_vec2 pos);
	bool point_meets_threshold(_vec2 pos);

	bool check_upcoming_stop(size_t * stop_pos);
	bool check_upcoming_turn(size_t index, size_t * turn_pos, float* turn_angle);
	bool check_turn_side(size_t index, float* angle);
	float compute_angle(size_t index, _vec2 curr_pos, _vec2 next_pos, size_t threshold);
	_vec2 irl_scale(_vec2 pos);
	float irl_scale(float dist);
	_vec2 map_scale(float dist_cm, int * pop, float* remainder);
};