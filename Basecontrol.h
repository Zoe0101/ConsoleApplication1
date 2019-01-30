

#pragma once
#include "stdafx.h"
#include<stdio.h>
#include "SerialClass.h"

using namespace std;



class BaseControl
{
public:
	//Port
	char *comPort="COM3";
	Serial* arduino;
	//constructor
	BaseControl();
	~BaseControl();
	
	//read baudrate from arduino 
	int BaseControl::getBaudrate();
	//get encoder counts
	int BaseControl::getEncoderCounts();
	//reset encoders
	void BaseControl::resetEncoders();
	
	//Driving motors by using speed
	void BaseControl::driveWithSpeed(float right, float left);
	//Read sonar sensor
	int BaseControl::ReadSornaSensor();
	//move robot with linear distance
	//the speed is setted at 0.3m/s
	void BaseControl::WalkRobot(double distance, int direction);
	void BaseControl::WalkRobotBySpeedLeftTick(float left, float right, int tick);
	//rotate robot with a certain angle
	//the angular speed is set at 1 rad/s
	void BaseControl::RotateRobot(double angle, int direction);
	//walk the robot in follow mode
	void BaseControl::Follow();
	//Circle the robot around human with certain degree
	void BaseControl::CircleRobot(double radius, double angle, int direction);
	//Pass an obstacle
	bool PassObstacle(int direction);
	double BaseControl::rotateX(float x,float y,double a);
	double BaseControl::rotateY(float x,float y,double a);
	//onekeytest
	void BaseControl::OneKeyTest(float dist,int dire);
	void BaseControl::GoToXY(float x,float y);
	void BaseControl::GoToXYwithoutRotation(float xx,float yy);
	void BaseControl::Ellipse(float a,float b);
	void BaseControl::Ass(float a,float b);

private:
	//hardware information
	const double wheel_diameter = 0.125;//m
	const double wheel_track = 0.34;//m
	const double encoder_resolution = 1200;
	const double gear_reduction = 1;
	const int encoder_min = -32768;
	const int encoder_max = 32768;
	const int servo_max = 180;
	const int servo_min = 0;
	//speed and acceleration limits
	const double speed_lim_inf = 0.1;
	const double speed_lim_sup = 0.7;
	const double accel_lim_inf = 0.5;
	const double accel_lim_sup = 3.0;
	//pid parameters
	int Kp = 50;
	int Kd = 20;
	int Ki = 0;
	int Ko = 50;
	//PID control rate
	const int pid_rate = 30;
	const int pid_interval = 1 / 30;
	//if arduino didn't receive commands in 2s, it stops
	const int timeout = 500;//ms
	//angular velocity(rad/s) to linear velocity factor
	const double A2L = wheel_track / 2;
	//walking speed
	const float Walking_Speed = 0.3;
	//read buffer
	char InComingData[64];
	//encoder counts recorder
	int e[2];
	//record sornar sensor data
	int sornar[4];
	
	//Driving motors
	HRESULT BaseControl::drive(int right, int left);
	//stop robot
	void BaseControl::stop();
	//sending the command to arduino board
	bool sendCommand(char *cmd);
	//Read data from ardunio
	bool readingData(int dataLen);
	

};
