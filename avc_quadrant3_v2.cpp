#include "E101.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <iostream>

void delay  (unsigned int milliseconds){	
	clock_t start = clock();
	while((clock() - start) * 1000 / CLOCKS_PER_SEC < milliseconds);;
}
using namespace std;

class Robot {
 
	public: //variables to use in robot methods
	
		const int scan_row = 120;
		const int v_left_go = 49; //velocity when left motor should go forwards
		const int v_right_go = 45; //velocity when right motor should go forwards (in opposite direction)
	        double v_left, v_right, dv, quadrant;
		double line_error; //how far away the black line is from the center
		double kp = 0.005; //proportional coefficient
		int check =  1 ;  
 
		 //methods to call on robot
		
		Robot (){};
		const int cam_width = 320;
		const int cam_height = 240;
		double  whi[320]; //array of 1's and 0's, 1 meaning black 0 meaning white
		double  whi1[320]; //array of 1's and 0's, 1 meaning black 0 meaning white
		void InitHardware(); //initialises hardware (not done this)
		bool line_present = true; //true if line is present
		void SetMotors(); //sets motors to right velocity
		bool OpenGate(); //opens the gate in first quadrant
		int MeasureLine();
		void FollowLine();
		void FollowLine2();
		void TurnLeft();
		void TurnRight();
		int MeasureLine2();

};

void Robot::InitHardware() {
	v_right = 47;
	v_left = 47;
	SetMotors();
}

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

int Robot::MeasureLine2() {

	take_picture();
	//cout<<"Picture passed"<<endl;
	update_screen();
	
 /** Creates an array then stores white pixels in that array */
	int max = 0;
	int min = 255;
	int min1 = 255;
	int max1 = 0;

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
		int pixel1 = get_pixel(0, i, 3); // get pixel is not working
		if (pixel1 > max1){
			max1 = pixel1;
		}
 
		if (pixel1 < min1){
			min1 = pixel1;
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
	
	for (int b = 0; b < cam_width; b++){ //replaces value with a 1 if above threshold
		
		whi1[b]=0;
		double pix2 = get_pixel(0,b,3);
		if (pix2 > thr){
			whi1[b]=1;
		}
		
	}

	line_present = 1;
	line_error = 0;
	int nblack = 0;
	int nblack1 = 0;
	
	for ( int i = 0; i < cam_width; i++ ){
		if (get_pixel(scan_row,i,3)<=thr){
			line_error = line_error + (i-cam_width/2);
			nblack++;
		}
	}
	
	for ( int i = 0; i < cam_width; i++ ){
		if (get_pixel(0,i,3)<=thr){
			line_error = line_error + (i-cam_width/2);
			nblack1++;
		}
	}
	if (nblack  > 200 && nblack1 < 20  ) {
		TurnRight();
		std::cout<<"Turn Right"<< std::endl;
	}
	if (nblack == 0 ) {
		line_present =0;
		return -1;
	}
	line_present = 1;
	line_error = line_error/nblack;
	return 0;
}

int Robot::MeasureLine() {

	take_picture();
	//cout<<"Picture passed"<<endl;
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

	line_present = 1;
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
		//cout << " line error = " << line_error << " dv=" << dv;
		SetMotors();
	}

 
	else if (line_present==0) { //if line isn't present

		std::cout <<" Line missing ---------------------- "<< std::endl;
		v_left = 44;
		v_right = 50;
		SetMotors();
		sleep1(100);
	}
}

void Robot::FollowLine2() {
	MeasureLine2();
	if (line_present==1){
		double dv = line_error * kp; //sets the velocity depending on the error
		v_left = v_left_go + dv;
		v_right = v_right_go + dv;
		//cout << " line error = " << line_error << " dv=" << dv;
		SetMotors();	}

 
	else if (line_present==0) { //if line isn't present

		std::cout <<" Line missing ---------------------- "<< std::endl;
		v_left = 44;
		v_right = 50;
		SetMotors();
		sleep1(100);
	}
}


void Robot::TurnLeft(){
	int count = 0;	
	SetMotors();
while (count<300){
	count = count +1;
	v_right = 45;
	v_left = 45;
	SetMotors();
}
	v_right = 47;
	v_left = 47;
	SetMotors();
	FollowLine2();
}

void Robot::TurnRight(){
	int count = 0; 
	delay(300);
	v_right = 47;
	v_left = 47;
	SetMotors();
	delay(1500);
	v_right = 49;
	v_left = 49;
	SetMotors();
	delay(1700);
	v_right = 47;
	v_left = 47;
	SetMotors();
	delay(1500);
	FollowLine2();
}

void Robot::SetMotors(){
	set_motors(1,v_right); //Need to be adjusted
	set_motors(5,v_left);
	hardware_exchange(); 
}

int main() {

	Robot robot;
	init(0);
  	robot.InitHardware();
	open_screen_stream(); //connects to the camera stream
	std::cout <<" Test 1 "<< std::endl;
	int quadrant = 1;
	
	robot.OpenGate();
	std::cout <<" Test 2 "<< std::endl;
	quadrant = 3;
		
//	while (quadrant == 2){
	//	std::cout <<" Test 3 "<< std::endl;
	//	robot.FollowLine();
  //  }
    while (quadrant == 3 ){
		robot.FollowLine2();		
	}
	close_screen_stream();
	return 0;
}

