#ifndef __ALL_INCLUDES__
#define __ALL_INCLUDES__

//****************************************
//				Defines
//****************************************
#define VERBOSE								//m_Print will print to console
//#define LOGFILE							//m_Log will log to text file



//****************************************
//				All includes
//****************************************
#include <iostream>




//****************************************
//				My headers
//****************************************
#include "fsm.h"
#include "Timer.h"
#include "BluetoothManager.h"
#include "SpeedEncoder.h"
#include "IFSensor.h"
#include "Accelerometer.h"
#include "Camera.h"
#include "Driver.h"
#include "Autopilot.h"



//****************************************
//				GLOBAL Functions
//****************************************
void m_Print(const char msg[]);
void m_Log(const char msg[]);
void m_Pause();



#endif //__ALL_INCLUDES__