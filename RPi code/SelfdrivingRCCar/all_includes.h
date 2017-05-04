#ifndef __ALL_INCLUDES__
#define __ALL_INCLUDES__

//****************************************
//				Defines
//****************************************
#define VERBOSE								//m_Print will print to console
//#define LOGFILE							//m_Log will log to text file
#define CONFIG_FILENAME "autopilotconfig.txt"
#define BUFLEN 128
//#define USE_XBEE


//****************************************
//				Global Vars
//****************************************

extern int OBSTACLE_THRESHOLD,      //cm
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
			TURN_DIST_THRESHOLD,
			TURN_DUTY_MAX,
			STOP_DIST_THRESHOLD,
			FWD_DUTY_MAX,
			REVERSE_DUTY_MAX,
			FWD_DUTY_MIN,
			FWD_DUTY_AVG,
			REVERSE_DUTY_MIN,
			REVERSE_DUTY_AVG,
			SPEED_CONTROL_TIME,
			TURN_TRAVEL_DISTANCE,
			OBSTACLE_CONSECUTIVE_COUNT,
			MUSIC_PIN,
			MUSIC_FREQ;
	
extern float WHEEL_CIRCUMFERENCE, //cm
			  WHEEL_RADIUS,		   //cm
			  MAX_TURN_ANGLE,
			  LOOP_INTERVAL_TIME,
			  ADJUST_ANGLE_SENSITIVITY, //degrees
			  Kp,
			  Ki,
			  Kd;


//****************************************
//				All includes
//****************************************

#include <softPwm.h>
#include <wiringPi.h>
#include <pigpio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <xbeep.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <mutex>
#include <fstream>
#include <time.h>
#include <thread>
#include <chrono>

//57 fwd, 52 reverse


struct my_MSG
{
	std::string type = "";
	int unique_id = -1;
	std::string message = "";
	int num = -1;
};


//****************************************
//				My headers
//****************************************
#include "fsm.h"
#include "pid.h"
#include "Timer.h"
#include "ConnectionManager.h"
#include "SpeedEncoder.h"
#include "ObstacleDetection.h"
#include "Driver.h"
#include "LaneDetection.h"
#include "Autopilot.h"



//****************************************
//				GLOBAL Functions
//****************************************
void m_Print(const char msg[]);
void m_Log(const char msg[]);
void m_Pause();



#endif //__ALL_INCLUDES__
