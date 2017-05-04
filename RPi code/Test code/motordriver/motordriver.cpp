//test motordriver here
//compile: g++ -Wall -c "%f"
//build: g++ -Wall -o "%e" "%f" -lwiringPi -pthread  or -lpigpio

//#define __SHIT_LIBRARY__


#ifdef __SHIT_LIBRARY__
	#include <softPwm.h>
	#include <wiringPi.h>
	
#else
	#include <pigpio.h>	
#endif

#include <stdio.h>
#include <iostream>

#define MotorEnable 12
#define ServoMotorEnable 13

int MotorDuty = 0;
int ServoDuty = 0;

int ServoFreq = 50; //Hz
int MotorFreq = 490;//490; //Hz


bool ServoSetDuty(int target) {	
	
	if (target == 0 || (target >= 1000 && target <= 2000)) {
		ServoDuty = target;
		
		#ifdef __SHIT_LIBRARY__
			pwmWrite(ServoMotorEnable,ServoDuty);
		#else
			//gpioHardwarePWM(ServoMotorEnable, ServoFreq, ServoDuty * 10000);
			gpioServo(ServoMotorEnable, ServoDuty); // best soln
			//std::cout << "Servo Real range: " << gpioGetPWMrealRange(ServoMotorEnable) << "\n";
			//std::cout << "Real freq: " << gpioGetPWMfrequency(ServoMotorEnable) << "\n";
		
		#endif
		//std::cout << "Duty set: "<< duty <<"\n";
		return true;	
	}
	
	//std::cout << "Invalid duty target: " << target << "\n";
	return false;
}

bool MotorSetDuty(int target) {	
	
	if (target >= 0 && target <= 1000000000000) {
		MotorDuty = target;
		
		#ifdef __SHIT_LIBRARY__
			pwmWrite(MotorEnable, MotorDuty);
		#else
			gpioHardwarePWM(MotorEnable, MotorFreq, MotorDuty);
			std::cout << "Motor Real range: " << gpioGetPWMrealRange(MotorEnable) << "\n";
			std::cout << "Real freq: " << gpioGetPWMfrequency(MotorEnable) << "\n";
		#endif
		//std::cout << "Duty set: "<< duty <<"\n";
		return true;	
	}
	
	//std::cout << "Invalid duty target: " << target << "\n";
	return false;
}

void Accelerate() {
	MotorDuty++;
	
	if (MotorSetDuty(MotorDuty)) {
	} else {
		MotorDuty--;
	}
}

void Decelerate() {
	MotorDuty--;
	
	if (MotorSetDuty(MotorDuty)) {
	} else {
		MotorDuty++;
	}
}

void ImmediateStop() {
	
	MotorDuty = 0;
	MotorSetDuty(0);
}

//10 - max
void TurnRight(int target) {
	
	printf("Turn Right..\n");
	ServoSetDuty(target);
}

//20 - max
void TurnLeft(int target) {
	
	printf("Turn Left..\n");
	ServoSetDuty(target);
}

//15
void SetStraight(int target) {
	
	printf("Straight..\n");
	ServoSetDuty(target);
}

#ifdef __SHIT_LIBRARY__
void ActivateMotor() {
	MotorSetDuty(50);
	delay(250);	
	MotorSetDuty(0);
	delay(250);
}
#endif
		
	
void Initialize() {	

	#ifdef __SHIT_LIBRARY__
	
		pinMode(MotorEnable, PWM_OUTPUT);	
		pwmSetMode(PWM_MODE_MS);
		pwmSetClock(786);//786
		pwmSetRange(50);//50   
		
		/*pinMode(ServoMotorEnable, PWM_OUTPUT);
		pwmSetMode(PWM_MODE_MS);
		pwmSetClock(384);
		pwmSetRange(1000);*/
		
		pinMode(ServoMotorEnable, OUTPUT);	
		softPwmCreate(ServoMotorEnable, 0, 100);
	
	#else
	
		gpioSetMode(MotorEnable, PI_OUTPUT);
		gpioSetMode(ServoMotorEnable, PI_OUTPUT);
	
	#endif

}


int main(void)
{
	#ifdef __SHIT_LIBRARY__
	
		if(wiringPiSetup() == -1)
		{
			printf("setup wiringPi failed !");
			return 1; 
		}
	#else
	
		if (gpioInitialise() < 0)
		{
		   printf("setup pigpio failed !");
		   //return 1;
		}
	
	#endif

	Initialize();
	
	
	#ifdef __SHIT_LIBRARY__
	
		//test servo
		//int i = 0;
		/*
		while(i < 5) {
			
			SetStraight(15);
			delay(1000);
			TurnRight(10);
			delay(1000);
			TurnLeft(20);		
			delay(1000);
			i++;
		}*/
		
		//i = 0;
		//test motor
		//SetStraight(15);
		//ActivateMotor();
		
		int target = 0;
		int servo_or_motor = 0;
		while (target != -1) {
			
			std::cout << "Choose servo (0) or motor (1): ";
			std::cin >> servo_or_motor;		
			
			std::cout << "Enter duty: ";  //35 and 34 is for the ideal forward speed
			std::cin >> target;
			std::cout << "\n";
			
			if (servo_or_motor == 0) {
				ServoSetDuty(target);
			} else {
				MotorSetDuty(target);
			}

		}
		
		//SetStraight(15);
		
	#else
	
		int target = 0;
		int servo_or_motor = 0;
		while (target != -1) {
			
			std::cout << "Choose servo (0) or motor (1): ";
			std::cin >> servo_or_motor;		
			
			std::cout << "Enter duty: ";  //35 and 34 is for the ideal forward speed
			std::cin >> target;
			std::cout << "\n";
			
			if (servo_or_motor == 0) {
				ServoSetDuty(target);
			} else {
				MotorSetDuty(target * 1000);
			}

		}
	
	
	#endif
	
	return 0;
}



