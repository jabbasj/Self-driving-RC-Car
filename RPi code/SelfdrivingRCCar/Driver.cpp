#include "all_includes.h"


Driver::Driver() {

	//Initialize();
	INITIALISED = false;
	MotorDuty = 0;
	ServoDuty = 1500;
	servo_angle = 0;

	m_Print("Driver created");

}

Driver::~Driver() {

	//m_Print("Driver destroyed");
	//MotorSetDuty(0, true);
	//ServoSetDuty(0, true);
	
	//gpioTerminate();
}

void Driver::Initialize() {
	
	/*
	pinMode(MotorEnable, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(MotorClockTime);//960
	pwmSetRange(MotorRange);//50   
	
	pinMode(ServoMotorEnable, OUTPUT);	
	softPwmCreate(ServoMotorEnable, 15, 100);*/
	
	gpioSetMode(MotorEnable, PI_OUTPUT);
	gpioSetMode(ServoMotorEnable, PI_OUTPUT);
}



void Driver::SetAngle(double angle) {
	
	if (angle > MAX_TURN_ANGLE) {
		angle = MAX_TURN_ANGLE;
	} else if (angle < - MAX_TURN_ANGLE) {	
		angle = - MAX_TURN_ANGLE;
	}
	
	int duty = - angle / MAX_TURN_ANGLE * 500 + 1500;
	
	//std::cout << "Set Angle: " << angle << ", duty calculated: " << duty << "\n";
	Turn(duty);
}

double Driver::getAngle() {
	
	double angle = -((ServoDuty - 1500) / 500.0) * MAX_TURN_ANGLE;
	
	return angle;
}

//1000: right, 1500: straight, 2000: left
void Driver::Turn(int target) {
	
	ServoSetDuty(target);
}



bool Driver::TurnLeft(bool force /*=false*/) {
	
	if (ServoDuty == 0) {
		ServoDuty = 1500;
	}
	
	ServoDuty+=100;
	
	if (ServoSetDuty(ServoDuty, force)) {
		return true;
	}
	
	ServoDuty-=100;
	return false;
}

bool Driver::TurnRight(bool force /*=false*/) {
	
	if (ServoDuty == 0) {
		ServoDuty = 1500;
	}
	
	ServoDuty-=100;
	
	if (ServoSetDuty(ServoDuty, force)) {
		return true;
	}
	
	ServoDuty+=100;
	return false;
}


void Driver::SetStraight() {
	
	ServoSetDuty(1500);
}




void Driver::ImmediateStop() {
	
	if (!INITIALISED) {
		MotorSetDuty(REVERSE_DUTY_MAX);
	}
	delay(50);
	MotorSetDuty(0);
	delay(50);
	ServoSetDuty(1500);
	delay(50);
	ServoSetDuty(0);
	
	INITIALISED = true;
}
	

bool Driver::Accelerate(bool force /*=false*/) {
	
	//std::cout << "Accelerate..";
	
	MotorDuty++;
	
	if (MotorSetDuty(MotorDuty, force)) {
		//std::cout << "ok: " << MotorDuty << "\n";
		return true;
	}
	
	MotorDuty--;
	//std::cout << "denied: " << MotorDuty << "\n";
	return false;
}

bool Driver::Decelerate(bool force /*=false*/) {

	//std::cout << "Decelerate..";

	MotorDuty--;

	if (MotorSetDuty(MotorDuty, force)) {
		//std::cout << "ok: " << MotorDuty << "\n";
		return true;
	}
	
	MotorDuty++;
	//std::cout << "denied: " << MotorDuty << "\n";
	return false;
}

void Driver::SetForward() {
	MotorSetDuty(FWD_DUTY_AVG);
}


bool Driver::MotorSetDuty(int target, bool force /*=false*/) {	
	
	if (target == 0 || (target >= REVERSE_DUTY_MAX && target <= FWD_DUTY_MAX) || (force && target <= TURN_DUTY_MAX)) {
		MotorDuty = target;
		
		//pwmWrite(MotorEnable,MotorDuty);
		gpioHardwarePWM(MotorEnable, MOTOR_FREQ, MotorDuty * 1000);
		std::cout << "Duty set: "<< MotorDuty <<"\n";
		return true;	
	}
	
	//std::cout << "Invalid duty target: " << target << "\n";
	return false;
}


bool Driver::ServoSetDuty(int target, bool force /*=false*/) {	
	
	if (target == 0 || (target >= 1000 && target <= 2000) || force) {
		ServoDuty = target;
		//servo_angle = getAngle();
		
		gpioServo(ServoMotorEnable, ServoDuty);
		
		servo_angle = getAngle();
		//std::cout << "Duty set: "<< duty <<"\n";
		return true;	
	}
	
	//std::cout << "Invalid duty target: " << target << "\n";
	return false;
}






void Driver::TestServo(int count /*= 1*/) {
	m_Print("\nServo Test!");
	
	for (int i = 0; i < count; i ++) {
	
		SetAngle(MAX_TURN_ANGLE);
		delay(1000);
		
		SetStraight();
		delay(2000);
		
		SetAngle(-MAX_TURN_ANGLE);
		delay(1000);
		
		SetStraight();
		
	}
	
	m_Print("Servo Test finished...");	
}

void Driver::TestAcceleration(int count /*= 1*/) {
	
	m_Print("\nTesting Acceleration!");	
	SetForward();
	
	for (int i = 0; i < count; i++) {
		if (!Accelerate()) {
			break;
		}
		delay(400);
	}
	
	for (int j = 0; j < count; j++) {
		if (!Decelerate()) {
			break;
		}
		delay(300);
	}
	
	ImmediateStop();	
}


void Driver::TestDeceleration(int count){
	
	m_Print("\nTesting Decelerating!");
	
	for (int i = 0; i < count; i++) {
		if (!Decelerate()) {
			break;
		}
		delay(300);
	}

}
		
	

void Driver::TestReverse(int count){
	
	m_Print("\nTesting Reverse!");
	
	//SetBackward();
	TestAcceleration(count);	
	
}



void Driver::TestConstantSpeed(int count){
	
	m_Print("\nTesting Constant Speed!");
			
	SetForward();
			
	MotorSetDuty(41); // For now set it to 40% of motor original speed
	delay(3000);
	
	ImmediateStop();
	
}





	
	

