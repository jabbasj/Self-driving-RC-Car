#include "all_includes.h"


Autopilot::Autopilot() {

	m_Print("Autopilot created");

	ready_to_start = false;
	init_transitions();

}

Autopilot::~Autopilot() {

	//m_Print("Accelerometer destroyed");

}


void Autopilot::init_transitions() {

	m_Print("Transitions defined");

	fsm.add_transitions({
		//  from state ,     to state  ,      triggers    ,       guard/condition      ,      action
		{ States::REST, States::STARTING, Triggers::start, [&]{return ready_to_start; }, [&]{start_autopilot(); } },
	});
}


void Autopilot::start_autopilot() {	

	m_Print("Starting autopilot");
}


void Autopilot::Start() {

	fsm.execute(Triggers::start);
}

const char* Autopilot::GetCurrentState() {

	return stringify_state(fsm.state());
}

const char* Autopilot::stringify_state(States state) {

	switch (state) {
	case States::REST:
		return "REST";
	case States::STARTING:
		return "STARTING";
	case States::CRUISING:
		return "CRUISING";
	case States::RECOVERING:
		return "RECOVERING";
	case States::TURNING:
		return "TURNING";
	case States::STOPPING:
		return "STOPPING";
	case States::STOPPED:
		return "STOPPED";
	case States::AVOIDING_COLLISION:
		return "AVOIDING_COLLISION";
	default:
		return "?????unknown state??????";
	}
}