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
 
	public:
	
		const int scan_row = 120;
		int v_left_go = 51.7; //velocity when left motor should go forwards
		int v_right_go = 45; //velocity when right motor should go forwards (in opposite direction)
	        double v_left, v_right, dv;
		double line_error; //how far away the black line is from the center
		double kp = 0.01; //proportional coefficients
		int dl,quadrant;
		double red,green,blue;
		int countl = 0;
		int noturn =0;
		int check1 = 0;
		int check2=0;
		double red4,green4,blue4;
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
		void TurnLeft();
		void TurnRight();
		void IsRed();
		void KeepStraight();
		void UTurn();
		void CameraUp();
		void TrackRed();
};

void Robot::InitHardware() { //method to initialise hardware to at begining
	v_right = 47;
	v_left = 47;
	SetMotors();
}

bool Robot::OpenGate() { //method that opens the gate by connecting to server and getting a password

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

int Robot::MeasureLine() { //method to measure the presence of the black line
	take_picture();
	update_screen();
	
 /** Creates an array then stores pixels in that array */
	int max = 0;
	int min = 255;
	int min1 = 255;
	int max1 = 0;

	for (int i = 0; i < cam_width; i++){
		int pixel = get_pixel(scan_row, i, 3); 
		if (pixel > max){
			max = pixel;
		}
 
		if (pixel < min){
			min = pixel;
		}
    }
    
    for (int i = 0; i < cam_width; i++){
		int pixel1 = get_pixel(0, i, 3);
		if (pixel1 > max1){
			max1 = pixel1;
		}
 
		if (pixel1 < min1){
			min1 = pixel1;
		}
	}
	
	double thr = (max+min)/2.4; //calculates threshold
	
	for (int a = 0; a < cam_width; a++){ //replaces value with a 1 if above threshold
		whi[a]=0;
		double pix = get_pixel(scan_row,a,3);
		if (pix > thr){
			whi[a]=1;
		}
		
	}
	
	for (int b = 0; b < cam_width; b++){ 
		
		whi1[b]=0;
		double pix2 = get_pixel(0,b,3);
		if (pix2 > thr){
                int dl= 0 ;
			whi1[b]=1;
		}
		
	}

	line_present = 1;
	line_error = 0;
	int nblack = 0;
	int nblack1 = 0;
	int c1 = 0;
	int b1 = 0;
	
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
	
	for  (int i = 0; i<30; i++){
		if (get_pixel(scan_row,i,3)<=thr){
			c1++;
		}
	}
	for (int ii = 290; ii <cam_width; ii++){
		if (get_pixel(scan_row,ii,3)<=thr){
			b1++;
		}
	}

	int rat2 = c1-b1; //ratio

	std::cout << nblack<<std::endl;
	if (nblack  > 130  && quadrant == 3 && noturn == 0 && countl == 0 && check1 == 1  && check2 == 0 ) {
  		TurnRight();
		noturn = 1;
		countl=1;		
	}
	if (nblack >130 && quadrant == 3 && noturn == 1 && countl == 0 && check2 == 0 ) { 
		TurnRight();
		noturn = 2;
		countl=1;
	}
	if (nblack >130 && quadrant == 3 && noturn == 2	&& countl == 0 && check2 == 0){
		TurnLeft();
		noturn = 3;
		countl = 1;
	}	
	if (nblack >130 && quadrant == 3 && noturn == 3 && countl == 0 && check2 == 0) {
		TurnLeft();
		noturn = 4;
		countl = 1;
	}
	if (nblack > 130 && quadrant == 3 && noturn == 4 && countl == 0 && check2 ==0 ){
		TurnRight();
		noturn = 5;
		countl=1;
	}
	if (nblack>130 && quadrant == 3 && noturn ==5 && countl == 0 && check2 == 0 ){
		TurnLeft();
		noturn = 6;
		countl=1;
	}
	if (nblack >130 && quadrant == 3 && noturn == 6 && countl == 0 && check2 ==0){
		TurnRight();
		noturn = 7;
		countl = 1;
	}
	if(nblack == 0){
		line_present = 0;
		return -1 ;
	}
	
	line_present=1;
	line_error = line_error/nblack;
	return 0;
}


void Robot::FollowLine() {
	if (quadrant == 2){
		kp = 0.01;
	}
	if (quadrant == 3){
		kp = 0.005;
		v_left_go = 51.3;
		v_right_go = 45.4;
	} 
	check1=1;
	MeasureLine();
	if (line_present==1){
		double dv = line_error * kp; //sets the velocity depending on the error
		v_left = v_left_go + dv;
		v_right = v_right_go + dv;
		SetMotors();
		if (quadrant ==3){
		}
		countl = 0;
		}
 
	else if (line_present==0 && quadrant == 2) { //if line isn't present
		
		v_left = 44;
		v_right = 51;
		SetMotors();
		sleep1(100);
	}
	else if (line_present == 0 && quadrant == 3){
		
	}
}

void Robot::TurnLeft(){ //method for a sharp left turn
	
	delay(900);
	v_right = 48;
	v_left = 48;
	SetMotors();
	delay(300);
	v_right = 43;
	v_left = 43;
	SetMotors();
	delay(800);
	check2 = 0;
	v_right = 48;
	v_left = 48;
	SetMotors() ; 
	delay(300);
	FollowLine();
	
}
void Robot::TurnRight(){ //method for a sharp right turn
	
	delay(900);
	v_right = 48;
	v_left = 48;
	SetMotors();
	delay(200);
	check2 = 1;
	v_right = 51;
	v_left = 53;
	SetMotors();	
	check2 = 0;
	delay(750);
	v_right = 48;
	v_left = 48;
	SetMotors();
	delay(300);
	FollowLine();
	
}

void Robot::CameraUp(){ //method to lift camera
	set_motors(3,56.5);
	hardware_exchange();
	}


void Robot::IsRed(){ //method for detecting read, used for switching to quadrants 3 and 4
	for (int i = 120; i <= 200 ; i++){
		red =  get_pixel(scan_row,i,0);
		green =  get_pixel(scan_row,i,1);
		blue =   get_pixel(scan_row,i,2);	
		double rat = red/green;
	if (rat >2){
		if ( quadrant == 2 && dl == 1  ) {
		v_left = 48;
		v_right = 48;
		SetMotors();
		delay(100);	
		quadrant = 3;

		std::cout<<"Quarant 3"<<std::endl;
			dl = 0; 
	}
		else if (quadrant == 3 && dl ==1  ) {
		v_left = 48;
		v_right = 48;
		SetMotors();
		delay(100);	
		quadrant = 4;
		std::cout<<"Quarant 4"<<std::endl;
			dl = 0;
	}
	}
}
}
void Robot::SetMotors(){ //method to set motors after new speed has been set
	set_motors(1,v_right);
	set_motors(5,v_left);
	hardware_exchange(); 
}

void Robot::KeepStraight(){ //keeps robot in a straight line
	v_left=52.3;
	v_right=44.8;
	SetMotors();
}

void Robot::TrackRed(){ //quadrant 4 for tracking red, not working
	int nRed = 0;
	double rat1 = 1;
	double rat2 = 1;
	double rat3 = 1;
	v_right = 51;
	v_left = 53.5;
	SetMotors();
	while (nRed < 10){
	for(int i = 0; i<320; i++){
		red4 = get_pixel(scan_row,i,0);
		green4 = get_pixel(scan_row,i,1);
		blue4 = get_pixel(scan_row,i,2);
		rat1 = red4/green4;
		rat2 = red4/blue4;
		rat3 = green4/blue4;
	if (rat1>2 && rat2>2){
		nRed = nRed++;
	}
	}
	std::cout<<red4<<std::endl;
	}
	if (nRed>10){
		KeepStraight();
	}
}



int main() {

	Robot robot;
	init(0);
  	robot.InitHardware();
	set_motors(3,30);
	hardware_exchange();
	open_screen_stream(); //connects to the camera stream
	robot.quadrant = 1;
	robot.dl=0;
	robot.OpenGate();
	int count = 0;
	robot.KeepStraight();
	delay(4000);
	robot.quadrant = 2;
	while (robot.quadrant == 2){
		if (robot.dl == 0){
		robot.KeepStraight();
		delay(3000);
		robot.dl = 1 ;
		}
	std::cout<<"Quadrant 2"<<std::endl;		
		if (robot.dl == 1) {
		robot.FollowLine();
		robot.IsRed();
		 }
	}
   	 while (robot.quadrant == 3 ){

		if (robot.dl == 0){
		robot.KeepStraight();
		delay(1000);
		robot.dl = 1;
		}	
		if (robot.dl==1){
		robot.FollowLine();
		robot.IsRed();
		
		}
	}
	if (robot.quadrant == 4){
		robot.CameraUp();
		robot.KeepStraight();
		delay(1000);
		robot.TrackRed();
	}
	close_screen_stream();
	return 0;
}
