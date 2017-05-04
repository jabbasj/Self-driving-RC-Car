#ifndef __AUTOPILOT__
#define __AUTOPILOT__

#include "all_includes.h"

class Autopilot {

public:
	Autopilot();
	~Autopilot();
	
	void Start();
	void Stop();
	const char* GetCurrentState();

//private:
	Timer m_Timer;
	ObstacleDetection m_ObstacleDetection;
	SpeedEncoder m_SpeedEncoder;
	LaneDetection m_LaneDetection;
	ConnectionManager * m_ConnectionManager;
	Driver m_Driver;
	libxbee::XBee * xbee;
	
	bool STARTED, FOLLOWING_PATH, CONNECTION_ACTIVE, TURN_INCOMING, STOP_INCOMING, OBSTACLE_DETECTED;
	
	float last_total_disp, turn_angle, speed, last_disp /*cm * 1000 */;
	int speed_count, turn_dist, map_turn_angle, stop_dist, obstacle_dist;
	
	std::thread THREAD_obstacleDetection;
	std::thread THREAD_laneDetection;
	std::thread THREAD_handleReceivedMessages;
	std::thread THREAD_calculateSpeed;
	std::thread THREAD_mainLoop;

	void HANDLE_ReceivedMessages();
	void MAIN_LOOP();

	enum class States { REST, STARTING, CRUISING, RECOVERING, TURNING, STOPPING, STOPPED, AVOIDING_COLLISION, RECONNECTING, FINISHED};
	enum class Triggers { start, recovered, avoid_obstacle, obstacle_cleared, maintain, adjust_motion, hiccup, turn_ahead, turn_complete, stop_ahead, stop_complete, destination_reached, finished, reconnect};
	FSM::Fsm<States, States::REST, Triggers> fsm;
	void INIT_TRANSITIONS();

	void start_autopilot();
	void maintain();
	bool execute_turn();
	bool execute_stop();
	
	bool QUESTION_goForward();
	bool QUESTION_goBackward();
	bool QUESTION_turnAhead();
	bool QUESTION_obstacleFound();
	bool QUESTION_adjustAngle();
	bool QUESTION_stopIncoming();
	bool QUESTION_speedUp();
	bool QUESTION_slowDown();

	const char* stringify_state(States state);
	
	void SEND_DispMessage(int disp);
	void SEND_SpeakMessage(std::string state);
	float last_total_distance;
	void CalculateSpeed(bool * active); 
	int time_of_last_speed_update;
	
	bool READ_CONFIG();
	void wave(int delay_us, int seconds);
};


#endif //__AUTOPILOT__
