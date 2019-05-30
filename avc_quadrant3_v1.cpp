#include <stdio.h>
#include "E101.h"
#include <unistd.h>
#include <time.h>
#include <iostream>

using namespace std;

class Robot {
 
	private: //variables to use in robot methods
	
		const int scan_row = 120;
		const int scan_row1 = 0;
		int check = 1;
		const int scan_row2 = 240;
		const int v_left_go = 52; //velocity when left motor should go forwards
		const int v_right_go = 44; //velocity when right motor should go forwards (in opposite direction)
	    double v_left, v_right, dv;
	    int quadrant;
		double line_error; //how far away the black line is from the center
		double kp = 0.02; //proportional coefficient
		bool isRed = false; //variable to check if camera detects any read
 
	public: //methods to call on robot
		
		Robot (){};
		const int cam_width = 320;
		const int cam_height = 240;
		double  whi[320]; //array of 1's and 0's, 1 meaning black 0 meaning white
		double	whitop[320];
		double	whibot[320];
		int InitHardware(); //initialises hardware (not done this)
		bool line_present = true; //true if line is present
		void SetMotors(); //sets motors to right velocity
		bool OpenGate(); //opens the gate in first quadrant
		int MeasureLine();
		void FollowLine();
		void TurnLeft();
		void TurnRight();
		void KeepStraight();
		void Detector();

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

int Robot::MeasureLine() {

	take_picture();
	update_screen();
	
 /** Creates an array then stores white pixels in that array */
	int max = 0;
	int min = 255;
	int max1 = 0;
	int min1 = 255;
	int max2 = 0;
	int min2 = 255;

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

	line_present = 1;
	if (min>80){
		line_present = 0;
	}
	line_error = 0;
	int nblack = 0;
	
	for ( int i = 0; i < cam_width; i++ ){
		if (get_pixel(scan_row,i,3)<=thr){
			line_error = line_error + (i-cam_width/2);
			nblack++;
		}
	}
	if (nblack == 0 ) {
		line_present =0;
		return -1;
	}
	line_present = 1;
	line_error = line_error/nblack;
	return 0;

}


void Robot::FollowLine() {
 
	MeasureLine();
	if (line_present) {
		
		double dv = line_error * kp; //sets the velocity depending on the error
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
void Robot::Detector(){
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
	
	for (int i = 0; i < cam_width; i++){
		int pixel1 = get_pixel(scan_row1, i, 3); // get pixel is not working
		if (pixel1 > max1){
			max = pixel;
		}
 
		if (pixel1 < min1){
			min1 = pixel1;
		}
 
	}
	
	for (int i = 0; i < cam_width; i++){
		int pixel2 = get_pixel(scan_row2, i, 3); // get pixel is not working
		if (pixel2 > max2){
			max2 = pixel2;
		}
 
		if (pixel2 < min2){
			min2 = pixel2;
		}
 
	}
	
	
	
	double thr = (max+min)/2; //calculates threshold
	
	if (check == 1){
	
	for (int a = 0; a < cam_width; a++){ //replaces value with a 1 if above threshold
		
		whi[a]=0;
		double pix = get_pixel(scan_row,a,3);
		if (pix > thr){
			whi[a]=1;
		}
		
	}

	for (int a = 0; a < cam_width; a++){ //replaces value with a 1 if above threshold
		
		whitop[a]=0;
		double pix1 = get_pixel(scan_row1,a,3);
		if (pix1 > thr){
			whitop[a]=1;
		}
		
	}
	
	for (int a = 0; a < cam_width; a++){ //replaces value with a 1 if above threshold
		
		whibot[a]=0;
		double pix2 = get_pixel(scan_row2,a,3);
		if (pix2 > thr){
			whibot[a]=1;
		}
		
	}
	}
	
	line_present = 1;
	if (min>80){
		line_present = 0;
	}
	line_error = 0;
	int nblack = 0;
	
	for ( int i = 0; i < cam_width; i++ ){
		if (get_pixel(scan_row,i,3)<=thr){
			line_error = line_error + (i-cam_width/2);
			nblack++;
		}
	}
	if (nblack == 0 ) {
		line_present =0;
		return -1;
	}
	line_present = 1;
	line_error = line_error/nblack;
	return 0;
}
void Robot::TurnLeft(){
	v_left = 48;
	v_right = 55;
	SetMotors();
}
void Robot::TurnRight(){
	v_left = 55;
	v_right = 48;
	SetMotors();
	
}
void Robot::KeepStraight(){
	v_left = 43;
	v_right = 53;
	SetMotors();
}

void Robot::SetMotors(){
	set_motors(1,v_right); //Need to be adjusted
	set_motors(5,v_left);
	hardware_exchange(); 
}

void Robot::DeadEnd(){ //method for quandrant 3
	

	
	
}

int main() {

	Robot robot;
	init(0);
  	//robot.InitHardware(); 
	open_screen_stream(); //connects to the camera stream
	int quadrant = 1;
	robot.OpenGate();
	quadrant = 2;
		
	while (quadrant == 2){
		
		robot.FollowLine();
		if (isRed()){
			
			quadrant = 3;
			
		}
		
    }

	close_screen_stream();
	return 0;

}
