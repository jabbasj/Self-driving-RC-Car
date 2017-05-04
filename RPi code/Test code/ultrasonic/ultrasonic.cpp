#include <stdio.h>
#include <wiringPi.h>
#include <iostream>

#define TRIG 15
#define ECHO 16
#define MAX_ATTEMPS 10000
#define MAX_RANGE 50 //cm

bool count = 0;
int obstacle_dist = 0;
bool obstacle_found = false;



 
void setup() {
        wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO, INPUT);
 
        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
}
 
int getCM() {
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



/*
static uint32_t startTick, firstTick = 0;
void sonarTrigger();
void sonarEcho(int gpio, int level, uint32_t tick);
int setup();
static int distance = 100000;*/


 
int main(void) {
	
	
	setup();
	
		
	while(1) {
			
		delayMicroseconds(250);
		obstacle_dist = getCM();
		
		
		if (obstacle_dist != 0) {
			
			if (obstacle_dist <= MAX_RANGE) {
				
				printf("Obstacle Distance: %dcm\n", obstacle_dist);
				count++;
				if (count > 10) {		
					obstacle_found = true;
				}
				
			} else {
				printf("Out of range\n");
				count = 0;
				obstacle_found = false;
			}
		}
	}
			
    return 0;
}

/*
void sonarTrigger() {
	
	
	//gpioWrite(TRIG, PI_ON);
	
	//gpioDelay(10); //10us trigger pulse
	
	//gpioWrite(TRIG, PI_OFF);
	
	gpioTrigger(TRIG, 10, PI_ON);
}

void sonarEcho(int gpio, int level, uint32_t tick) {	
	
	int diffTick;
	
	if (!firstTick) firstTick = tick;
	
	std::cout << "level: " << level << "on: " << PI_ON << "off: " << PI_OFF << "\n";
	
	if (level == PI_ON) {
		startTick = tick;
	}
	else if (level == PI_OFF) {
		diffTick = tick - startTick;
		
		distance = diffTick / 58;
		
		std::cout << "distance: " << distance << " cm\n";
	}
	
	
}


int setup() {
	
	
	if (gpioInitialise() < 0) {
		std::cout << "error initialise pigpio\n";
		return -1;
	}
	
	gpioSetMode(TRIG, PI_OUTPUT);
	gpioWrite(TRIG, PI_OFF);
	
	gpioSetMode(ECHO, PI_INPUT);
	
	gpioSetTimerFunc(0, 50, sonarTrigger);

	gpioSetISRFunc(0, EITHER_EDGE, 0, sonarEcho);
	//gpioSetAlertFunc(0, sonarEcho);
	
	
	
	return 0;
}
*/
