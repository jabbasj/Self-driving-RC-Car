#include "all_includes.h"

int SpeedEncoder::rotation = 0;
double SpeedEncoder::distance = 0;
double SpeedEncoder::speed = 0;
double SpeedEncoder::displacement = 0;

SpeedEncoder::SpeedEncoder() {
	
	m_Print("SpeedEncoder created");

}


SpeedEncoder::~SpeedEncoder() {

	//m_Print("SpeedEncoder destroyed");

}

void SpeedEncoder::SetInterrupt() {
	
	//generate an interrupt on high-to-low transitions on gpio SIG
	wiringPiISR(SIG, INT_EDGE_FALLING, &SpeedEncoder::InterruptHandler);
}

double SpeedEncoder::GetDisplacement() {
	double disp = displacement * 1000;	
	displacement = 0;
	
	return disp;
}

double SpeedEncoder::GetDistance() {
	return distance;
}

void SpeedEncoder::Reset() {
	
	rotation = 0;
	distance = 0;
	speed = 0;
	displacement = 0;	
}

void SpeedEncoder::InterruptHandler() {	

	static long start_time= micros();
	long elapsedtime;
	long interrupttime;		
		
	if(0 == digitalRead(SIG)){
		
		interrupttime = micros();
		rotation++;
		distance = rotation * WHEEL_CIRCUMFERENCE; //WHEEL_CIRCUMFERENCE is actually 1/4 of wheel circum.
		displacement += WHEEL_CIRCUMFERENCE;
		elapsedtime = interrupttime - start_time;
		
		speed = displacement/elapsedtime;
		
		start_time = interrupttime;			
			
	}
}
	

