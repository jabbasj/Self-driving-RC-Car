#ifndef __LANE_DETECTION__
#define __LANE_DETECTION__

#include "all_includes.h"

using namespace std;
using namespace cv;

class LaneDetection {
	friend class Autopilot;

public:
	LaneDetection();
	~LaneDetection();
	
	float GetAngle();
	
protected:

	void Loop(bool* active, double * servo_angle);

	VideoCapture livecamera;

	Point frame_center;
	Point closest_intercept_left; // X-intercept left of frame_center.x
	Point closest_intercept_right;

	Point pt1_l, pt2_l; //closest left line, line defined by 2 points
	Point pt1_r, pt2_r; //closest right line

	bool left_lane_found = false;
	bool right_lane_found = false;

	float last_total_angle = 0.0f;
	float target_angle_left = 0.0f;
	float target_angle_right = 0.0f;

	bool find_closest_left_right_intercepts(vector<Vec2f> lines);
	float compute_target_angle(bool left = true);
};


#endif //__LANE_DETECTION__
