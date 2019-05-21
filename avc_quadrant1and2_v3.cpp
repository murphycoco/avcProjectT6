#include <stdio.h>
#include "E101.h"
#include <unistd.h>
#include <time.h>
#include <iostream>

using namespace std;

class Robot {
 
	private: //variables to use in robot methods
	
		const int scan_row = 120;
		const int v_left_go = 52; //velocity when left motor should go forwards
		const int v_right_go = 44; //velocity when right motor should go forwards (in opposite direction)
		int v_left, v_right, dv, quadrant;
		double line_error; //how far away the black line is from the center
		double kp = 0.05; //proportional coefficient
 
	public: //methods to call on robot
		
		Robot (){};
		const int cam_width = 320;
		const int cam_height = 240;
		double  whi[320]; //array of 1's and 0's, 1 meaning black 0 meaning white
		int InitHardware(); //initialises hardware (not done this)
		bool line_present = true; //true if line is present
		void SetMotors(); //sets motors to right velocity
		void Error(); //calculates the error
		bool OpenGate(); //opens the gate in first quadrant
		void MeasureLine();
		void FollowLine();

};

bool Robot::OpenGate() {

	char message[24]="Please";
	char password[24];
	char IP[15]="130.195.6.196";
	connect_to_server(IP, 1024);
	send_to_server(message);
	receive_from_server(password);
	send_to_server(password);
	std::cout <<" Opened gate"<< std::endl;
	return true;

}

void Robot::MeasureLine() {

	take_picture();
	update_screen();
	
 /** Creates an array then stores white pixels in that array */
	int max = 0;
	int min = 255;

	for (int i = 0; i < cam_width; i++){
		int pixel = get_pixel(scan_row, i, 3); // get pixel is not working
		if (pixel > max){
			max = pixel;
		}
 
		if (pixel < min){
			min = pixel;
		}
 
	}
	
	double thr = (max+min)/2; //calculates threshold
	
	for (int a = 0; a < cam_width; a++){ //replaces value with a 1 if above threshold
		
		whi[a]=0;
		double pix = get_pixel(scan_row,a,3);
		if (pix > thr){
			whi[a]=1;
		}
		
	}


	for (int b = 0; b <cam_width; b++){ //if there is a black pixel in the array, returns
		
		if (whi[b]==1){
			line_present = true;
			break;
		}

	else{
		line_present = false;
		}

	}

}


void Robot::FollowLine() {
 
	MeasureLine();
	if (line_present) {

		dv = (int)(line_error * kp); //sets the velocity depending on the error
		v_left = v_left_go + dv;
		v_right = v_right_go + dv;
		cout << " line error = " << line_error << " dv=" << dv;
		SetMotors();
 
	}

 
	else { //if line isn't present

		std::cout <<" Line missing"<< std::endl;
		v_left = 39;
		v_right = 55;
		SetMotors();
		sleep1(100);

	}


}

void Robot::SetMotors(){
	
	set_motors(1,v_right); //Need to be adjusted
	set_motors(5,v_left);
	hardware_exchange(); 
}

void Robot::Error(){
	
MeasureLine();

	int e = 0;
	for (int i = -160; i < 0; i++){ //calculates error left of the center
		e = e + (whi[i] * i);
	}

	for (int i = 160; i > 0; i--){ //calculates error right of the center
	e = e + (whi[i] * i);
	}
		
	line_error = e;

}

int main() {

	Robot robot;
	init(0);
  	//robot.InitHardware(); 
	open_screen_stream(); //connects to the camera stream
	std::cout <<" Test 1 "<< std::endl;
	int quadrant = 1;
	
	robot.OpenGate();
	std::cout <<" Test 2 "<< std::endl;
	quadrant = 2;
		
	while (quadrant == 2){
		std::cout <<" Test 3 "<< std::endl;
		robot.FollowLine();

 		}

	close_screen_stream();
	return 0;

}
