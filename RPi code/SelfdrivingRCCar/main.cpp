#include "all_includes.h"

/* Commands to compile, build
 * 
 * compile: g++-4.8 -std=c++0x -Wall -g -pthread -c -o "%e.o" "%f"
 * build:  g++-4.8 -std=c++0x -Wall -g -pthread -lwiringPi $(pkg-config --libs --cflags opencv) -o "%e" "./pid.cpp" "Autopilot.cpp" "ConnectionManager.cpp" "LaneDetection.cpp" "Driver.cpp" "ObstacleDetection.cpp" "SpeedEncoder.cpp" "Timer.cpp" "./%e.o"
 * execute: sudo "./%e"
 */


/****************************************
				GLOBAL Variables
****************************************/
int 	OBSTACLE_THRESHOLD,      //cm
		CONSTANT_FORWARD_SPEED,  //cm/s ... or duty?
		CONSTANT_BACKWARD_SPEED, //cm/s
		PORT_NUM,
		MAX_DELAY,				//ms
		MOTOR_FREQ,
		MotorEnable,
		ServoMotorEnable,
		TRIG,
		ECHO,
		MAX_ATTEMPS,
		MAX_RANGE,				//cm
		SIG,
		TURN_DUTY_MAX,
		TURN_DIST_THRESHOLD,
		STOP_DIST_THRESHOLD,
		FWD_DUTY_MAX,
		REVERSE_DUTY_MAX,
		FWD_DUTY_MIN,
		FWD_DUTY_AVG,
		REVERSE_DUTY_MIN,
		SPEED_CONTROL_TIME,
		REVERSE_DUTY_AVG,
		TURN_TRAVEL_DISTANCE,
		OBSTACLE_CONSECUTIVE_COUNT,
		MUSIC_PIN,
		MUSIC_FREQ = 0;
	
float 	  WHEEL_CIRCUMFERENCE, //cm
		  WHEEL_RADIUS,		   //cm
		  MAX_ANGLE,
		  MAX_TURN_ANGLE,
		  LOOP_INTERVAL_TIME,
		  Kp,
		  Ki,
		  Kd,
		  ADJUST_ANGLE_SENSITIVITY = 0;

Autopilot m_Autopilot;


/****************************************
				MAIN
****************************************/
int main() {
	
	//m_Autopilot.m_Driver.TestAcceleration(50);
	//m_Autopilot.m_Driver.TestServo(1);

	m_Autopilot.Start();
	
	m_Pause();	

	return 0;
}



/****************************************
				GLOBAL Functions
****************************************/

void m_Print(const char msg[]) {

#ifdef VERBOSE
	std::cout << msg << std::endl;
#endif

}

//TODO: write this function for raspberry pi
void m_Log(const char msg[]) {

#ifdef LOGFILE
	//log msg into text file
#endif

}


void m_Pause() {
	std::cout << "Press enter to continue ...";
	std::cin.get();
}
