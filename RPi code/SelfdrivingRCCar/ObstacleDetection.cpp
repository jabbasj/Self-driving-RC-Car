#include "all_includes.h"


ObstacleDetection::ObstacleDetection() {

	m_Print("ObstacleDetection created");

	count = 0;
	obstacle_dist = 0;
	obstacle_found = false;
}


ObstacleDetection::~ObstacleDetection() {

	//m_Print("ObstacleDetection destroyed");

}


void ObstacleDetection::Loop(bool * active) {
	
	m_Print("Obstacle Detection Started...\n");
	
	setup();
	
	while(true) {
		
		while(!(*active));
			
		delayMicroseconds(250);
		obstacle_dist = getCM();
		
		
		if (obstacle_dist != 0) {
			
			if (obstacle_dist <= MAX_RANGE) {
				
				//printf("Obstacle Distance: %dcm\n", obstacle_dist);
				count++;
				if (count >= OBSTACLE_CONSECUTIVE_COUNT) {		
					obstacle_found = true;
				}
				
			} else {
				//printf("Out of range\n");
				count = 0;
				obstacle_found = false;
			}
		}
	}
	
	m_Print("Obstacle Detection finished...\n");
	
}

bool ObstacleDetection::CheckObstacle(int * dist) {
	
	*dist = obstacle_dist;
	return obstacle_found;
}


void ObstacleDetection::setup() {
	
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);

	//TRIG pin must start LOW
	digitalWrite(TRIG, LOW);
	delay(30);
}


int ObstacleDetection::getCM() {
	
	//Send trig pulse
	digitalWrite(TRIG, HIGH);
	delayMicroseconds(20);
	digitalWrite(TRIG, LOW);

	int attempts = 0;
	//Wait for echo start
	while(digitalRead(ECHO) == LOW) {
		if (++attempts >= MAX_ATTEMPS) {
			break;
		}
	}

	//Wait for echo end
	long startTime = micros();
	while(digitalRead(ECHO) == HIGH);
	long travelTime = micros() - startTime;

	//Get distance in cm
	int distance = travelTime / 58;

	return distance;
}
