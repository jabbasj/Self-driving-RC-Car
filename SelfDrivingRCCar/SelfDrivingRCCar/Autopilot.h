#ifndef __AUTOPILOT__
#define __AUTOPILOT__

#include "all_includes.h"

class Autopilot {

public:
	Autopilot();
	~Autopilot();

	bool ready_to_start; //todo: private

	void Start();
	const char* GetCurrentState();

private:
	Timer m_Timer;
	IFSensor m_IFSensor;
	SpeedEncoder m_SpeedEncoder;
	Accelerometer m_Accelerometer;
	Camera m_Camera;
	BluetoothManager m_BluetoothManager;
	Driver m_Driver;

	enum class States { REST, STARTING, CRUISING, RECOVERING, TURNING, STOPPING, STOPPED, AVOIDING_COLLISION};
	enum class Triggers { start, recovered, obstacle_cleared, maintain, adjust_motion, hiccup, turn_ahead, turn_complete, stop_ahead, stop_complete, destination_reached, finished };
	FSM::Fsm<States, States::REST, Triggers> fsm;
	void init_transitions();

	void start_autopilot();
	const char* stringify_state(States state);
};


#endif //__AUTOPILOT__