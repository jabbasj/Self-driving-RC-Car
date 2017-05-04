/*
 * Compile command:
 *  g++-4.8 -std=c++0x -Wall -g $(pkg-config --libs --cflags opencv) -o "%e" "./pid.cpp" "./%e.cpp"
*/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "pid.h"

using namespace std;
using namespace cv;

#define MAX_TURN_ANGLE 22.0f //degrees
#define LOOP_INTERVAL_TIME 0.1 //unit??
#define Kp 0.10  // proportional gain
#define Ki 0.01  // integral gain,
#define Kd 0.50  // derivative gain,

VideoCapture livecamera(0);

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

//PID( double dt, double max, double min, double Kp, double Kd, double Ki ); // Kp -  proportional gain, Ki -  Integral gain, Kd -  derivative gain, dt -  loop interval time, max - maximum value of manipulated variable, min - minimum value of manipulated variable
//PID.calculate( double setpoint, double pv );	// Returns the manipulated variable given a setpoint and current process value
PID pid = PID(LOOP_INTERVAL_TIME, MAX_TURN_ANGLE, -MAX_TURN_ANGLE, Kp, Ki, Kd);



int main()
{
	if (!livecamera.isOpened()) {
		cout << "cannot open camera";      // Webcam not opened 
		return 0;
	}
	
	livecamera.set(CV_CAP_PROP_FPS , 60);
	livecamera.set(3, 320);
	livecamera.set(4, 180);
	
	frame_center.x = livecamera.get(3) / 2; 
	frame_center.y = livecamera.get(4) / 2;

	while (true)
	{
		Mat cannyImage;
		Mat frame;
		livecamera.read(frame);		
		
		blur(frame, cannyImage, Size(3,3) );
		cvtColor(cannyImage, cannyImage, CV_BGR2GRAY);    // Converts from one one color space to another		
		Canny(cannyImage, cannyImage, 100, 200, 3);
		//imshow("Canny Transformation", cannyImage);

		vector<Vec2f> lines; 
		HoughLines(cannyImage, lines, 1, CV_PI / 180, 100);
		
		target_angle_left = 0.0f;
		target_angle_right = 0.0f;

		if(!find_closest_left_right_intercepts(lines)) {
			std::cout << "NO LANES\n";
		}
		
		if (left_lane_found) {
			std::cout << "Left lane found...\n";
			line(frame, pt1_l, pt2_l, Scalar(0, 0, 255), 3, CV_AA);
			
			target_angle_left = compute_target_angle(true); //left
		}
		
		if (right_lane_found) {
			std::cout << "Right lane found...\n";
			line(frame, pt1_r, pt2_r, Scalar(0, 0, 255), 3, CV_AA);
			
			target_angle_right = compute_target_angle(false); //right
		}
		
		float total_angle = target_angle_left - target_angle_right;	
		
								    //target_value, "sensor" value
		total_angle = pid.calculate(total_angle, last_total_angle); 
		
		std::cout << "total_angle: " << total_angle << "\n";		
		last_total_angle = total_angle;		
		
		imshow("Lane Detection", frame);  //opening the hough transformation
		int key = cv::waitKey(1);
		key = (key==255) ? -1 : key; //#Solve bug in 3.2.0
		if (key>=0)
		  break;

	}

	return 0;
}


bool find_closest_left_right_intercepts(vector<Vec2f> lines) {
	
	enum side {left, right, center};
	left_lane_found = false;
	right_lane_found = false;
	closest_intercept_left.x = 1000000.0f;
	closest_intercept_right.x = 1000000.0f;			
	
	//clear old points
	Point pt_c;
	pt1_l = pt_c;
	pt2_l = pt_c;
	pt1_r = pt_c;
	pt2_r = pt_c;

	for (size_t i = 0; i < lines.size(); i++) {
		double rho = lines[i][0], theta = lines[i][1];				       // rho variable is the distance resolution in pixels	
																		   // theta variable is angle resolution in radians
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));	

		double angle = theta * 180 / CV_PI;
		if (angle >= 180) {
			angle -= 180;
		}

		if (angle != 0) {
			
			Vec2f cart_line;
			cart_line[0] = (-cos(theta) / sin(theta));
			cart_line[1] = rho / sin(theta);			
			
			Point intercept;
			intercept.y = frame_center.y;
			intercept.x = (frame_center.y - cart_line[1]) / cart_line[0];
			side intercept_side = center;						
			
			if (intercept.x > frame_center.x) {
				intercept_side = right;
			}
			else if (intercept.x < frame_center.x) {
				intercept_side = left;
			}			
			
			if (intercept_side == left) {				
				
				if (abs(frame_center.x - intercept.x) < abs(frame_center.x - closest_intercept_left.x)) {
					left_lane_found = true;
					closest_intercept_left = intercept;
					pt1_l = pt1;
					pt2_l = pt2;			
				}					
			} else if (intercept_side == right) {			
				
				
				if (abs(frame_center.x - intercept.x) < abs(frame_center.x - closest_intercept_right.x)) {
					right_lane_found = true;
					closest_intercept_right = intercept;
					pt1_r = pt1;
					pt2_r = pt2;			
				}						
			}
		}
	}	
	
	return (left_lane_found || right_lane_found);
}



float compute_target_angle(bool left /*= true*/) {
	
	float target_angle = 0.0f;
	
	if (left) {		
		target_angle = MAX_TURN_ANGLE * closest_intercept_left.x / frame_center.x;
				
	} else {		
		target_angle = -MAX_TURN_ANGLE * closest_intercept_right.x / frame_center.x + 2*(MAX_TURN_ANGLE);
	}
	
	if (target_angle < 0) {
		target_angle = 0;
	}
		
	return target_angle;
}
