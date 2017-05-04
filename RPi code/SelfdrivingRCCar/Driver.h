#ifndef __DRIVER__
#define __DRIVER__

#include "all_includes.h"

class Driver {
	friend class Autopilot;

public:
	Driver();
	~Driver();
	
	int MotorDuty;
	int ServoDuty;
	double servo_angle;
	bool INITIALISED;
	
	void TestServo(int count = 1);
	void TestAcceleration(int count = 1);
	void TestDeceleration(int);
	void TestReverse(int);
	void TestConstantSpeed(int);

protected:
	
	void Initialize();
	
	bool Accelerate(bool force = false);
	bool Decelerate(bool force = false);
	void ImmediateStop();
	bool MotorSetDuty(int target, bool force = false);
	bool ServoSetDuty(int target, bool force = false);
	void SetAngle(double angle);
	void Turn(int target);
	void SetStraight();
	void SetForward();
	bool TurnRight(bool force = false);
	bool TurnLeft(bool force = false);
	double getAngle();


private:

};


#endif //__DRIVER__
