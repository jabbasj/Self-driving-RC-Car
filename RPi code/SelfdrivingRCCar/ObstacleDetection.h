#ifndef __OBSTACLEDETECTION__
#define __OBSTACLEDETECTION__

#include "all_includes.h"


class ObstacleDetection {
	friend class Autopilot;

public:
	ObstacleDetection();
	~ObstacleDetection();
	
		
protected:

	int obstacle_dist;
	bool obstacle_found;
	int count;

	bool CheckObstacle(int * dist);
	void Loop(bool* active);
	void setup();
	int getCM(); 

};


#endif //__OBSTACLEDETECTION__
