#ifndef __SPEED_ENCODER__
#define __SPEED_ENCODER__

#include "all_includes.h"


class SpeedEncoder {

public:
	SpeedEncoder();
	~SpeedEncoder();
	
	static int rotation;
	static double distance; // cm
	static double speed; // cm / us
	static double displacement; //cm

	static double GetDisplacement();
	static double GetDistance();
	
	void SetInterrupt();
	void Reset();

private:
	static void InterruptHandler();
	
};


#endif //__SPEED_ENCODER__
