#include "stdafx.h"
# include "Basecontrol.h"
#include "SerialClass.h"
#include <SDKDDKVer.h>
#include <math.h>

#define M_PI 3.14159 
//send command to Arduino and read sensor's state
//parameters: comPort, cmd
//
bool BaseControl::sendCommand(char *cmd)
{
	
	char cmdend[1] = { '\r' };
	unsigned int sendingByte = strlen(cmd);
	/*cout << sizeof(cmd) << endl;*/
	if (arduino->IsConnected())
	{
		//cout << "Ardunio is connected" << endl;
		
		//sending command to Arduino, if succeed return true
		if (arduino->WriteData(cmd, sendingByte) && arduino->WriteData(cmdend, 1))
		{
			Sleep(50);
			//cout << "Command has been sent correctly!" << endl;
			return true;
		}

	}
	else cout << "Arduino can not be connected" << endl;
	Sleep(50);
	return false;
}

//Read data from ardunio
bool BaseControl::readingData(int dataLen)
{
	//char InComingData[64];
	int i = 0;
	for (i = 0; i < 64; i++)
	{
		InComingData[i] = '\0';
	}
	int readResult = 0;
	if (arduino->IsConnected())
	{
		//cout << "Ardunio is connected when reading" << endl;
		//read data from Arduino, if succeed return true
		readResult = arduino->ReadData(InComingData, dataLen);
		if (readResult != 0)
		{
			cout << InComingData << endl;
  			Sleep(50);
			return true;
		}
		else
			cout << "No data has been read!" << endl;


	}
	else cout << "Arduino is disconnected when reading" << endl;
	return false;
}

//Read the arduino baudrate
int BaseControl::getBaudrate()
{
	HRESULT hr;
	char cmd[] = "b";
	//Serial* arduino = new Serial(comPort);
	hr = BaseControl::sendCommand(cmd);
	Sleep(50);
	if (SUCCEEDED(hr))
	{
		cout << "The Baudrate is: ";
		readingData(25);
	}
	//arduino->~Serial();
	return 0;
}

//get encoder counts
int BaseControl::getEncoderCounts()
{
	HRESULT hr;
	char cmd[] = "e";
	hr = BaseControl::sendCommand(cmd);
	Sleep(50);
	if (SUCCEEDED(hr))
	{
		cout << "The encoder counts are: ";
		readingData(25);
		int i = 0;
		int j = 0;
		string el;
		string er;
		for (i = 0;i<25; i++)
		{
			
			if (InComingData[i] != ' '&&j == 0)
			{
				el += InComingData[i];
				Sleep(2);
			}

			else if (j==0)
			{
				j = 1;
				continue;
			}
			if (InComingData[i] != '\r '&&j == 1)
				er += InComingData[i];
			
			else if (InComingData[i] == '\r'&&j == 1) 
			{ 
				j = 0;
				break; 
			}
			
		}
		//cout << el << " " << er << endl;
		stringstream ss;
		ss << el;
		ss >> e[0];
		ss.clear();
		ss<<er;
		ss >> e[1];
		//cout << e[0] << " " << e[1] << endl;

	}
	return 0;
}

//reset encoders
void BaseControl::resetEncoders()
{
	HRESULT hr;
	char cmd[] = "r";
	hr = BaseControl::sendCommand(cmd);
	if (SUCCEEDED(hr))
	{   
		cout << "Encoder has been reseted!" << endl;
		readingData(25);
	}
	return;
}

//Driving motors
//send speed command to motor, the unit is ticks per second
HRESULT BaseControl::drive(int left, int right)
{
	std::stringstream ss;
	string str1;
	string str2;
	ss << left;
	ss >> str1;
	ss.clear();
	ss << right;
	ss >> str2;
	string str3 = "m " + str1 + " " + str2;
	const char *str4 = str3.c_str();
	char cmd[20];
	for (int i = 0; i < 20; i++)
		cmd[i] = str4[i];
	Serial* arduino = new Serial(comPort);
	HRESULT hr;
	hr = BaseControl::sendCommand(cmd);
	if (SUCCEEDED(hr))
	{
		cout << "The motor is drived at a speed of " << "left:" << left << " " << "right:" << right;
		cout << endl;
		readingData(25);
	}
	arduino->~Serial();
	return hr;
}

//Driving motors by using speed, the uint is m/s
void BaseControl::driveWithSpeed(float left, float right)
{
	
	if ((abs(right)<speed_lim_inf || abs(right)>speed_lim_sup)&&right!=0)
	{
		cout << "The speed of right wheel is out of range!!! Please reset it!" << endl;
		if ((abs(left)<speed_lim_inf || abs(left)>speed_lim_sup)&&left!=-0)
		{
			cout << "The speed of left wheel is out of range!!! Please reset it!" << endl;
			return;
		}
		return;
	}
	HRESULT hr;
	//calculating the ticks per loop
	double left_rev_per_sec = left / (wheel_diameter*PI);
	double right_rev_per_sec = right / (wheel_diameter*PI);
	int right_ticks_per_loop = (int)round(right_rev_per_sec*encoder_resolution*gear_reduction/pid_rate);
	int left_ticks_per_loop = (int)round(left_rev_per_sec*encoder_resolution*gear_reduction/pid_rate);
	cout << "m " << left_ticks_per_loop << " " << right_ticks_per_loop << endl;
	hr = drive(left_ticks_per_loop, right_ticks_per_loop);
	//if (SUCCEEDED(hr)) cout << "Speed control is ok!" << endl;
	return;
}

//Read sonar sensor
int BaseControl::ReadSornaSensor()
{
	HRESULT hr;
	char cmd[] = "p";
	hr = BaseControl::sendCommand(cmd);
	if (SUCCEEDED(hr))
	{
		Sleep(600);
		cout << "The data of sornar sensor is: ";
		readingData(25);
		int i = 0;
		int j = 0;
		string Snar[4];
		
		for (i = 0; i<25; i++)
		{
			switch (j)
			{
			case 0:	
				if (InComingData[i] != ' ')
			{
				Snar[0] += InComingData[i];
				Sleep(2);
			}
				else 
				{
					j = 1;
					continue;
				}
				break;
			case 1:
				if (InComingData[i] != ' ')
				{
					Snar[1] += InComingData[i];
					Sleep(2);
				}
				else 
				{
					j = 2;
					continue;
				}
				break;
			case 2:
				if (InComingData[i] != ' ')
				{
					Snar[2] += InComingData[i];
					Sleep(2);
				}
				else
				{
					j = 3;
					continue;
				}
				break;
			case 3:
				if (InComingData[i] != '\r')
				{
					Snar[3] += InComingData[i];
					Sleep(2);
				}
				else
				{
					j = 4;
					break;
				}
				break;
			default:
				break;
			}
			//break;
		
		}
		//cout << Snar[0] << " " << Snar[1] <<" "<<Snar[2]<<" "<<Snar[3]<< endl;
		stringstream ss;
		ss << Snar[0];
		ss >> sornar[0];
		ss.clear();
		ss << Snar[1];
		ss >> sornar[1];
		ss.clear();
		ss << Snar[2];
		ss >> sornar[2];
		ss.clear();
		ss << Snar[3];
		ss >> sornar[3];
		//cout << sornar[0] << " " <<sornar[1] << " "<<sornar[2]<<" "<<sornar[3]<<endl;
	}
	return 0;
}
//stop robot
void BaseControl::stop()
{
	HRESULT hr;
	hr=drive(0, 0);
	if (SUCCEEDED(hr)) cout << "The robot is stopped!" << endl;
}

//constructor
BaseControl::BaseControl()
{
	//comPort = NULL;
}

//destructor
BaseControl::~BaseControl()
{
	//SafeRelease(comPort);
}

//move robot with linear distance
//the speed is setted at 0.3m/s
void BaseControl::WalkRobot(double distance,int direction)
{
	float vl;
	float vr;
	//move forward
	if (direction == 1)
	{
		vl =Walking_Speed;
		vr = Walking_Speed;
	}
	//move backward
	else if (direction == 0)
	{
		vl = -Walking_Speed;
		vr = -Walking_Speed;
	}
	else cout << "The walking direction is unknown!!" << endl;
	resetEncoders();
	//target encoder counts
	int desirePos = (int)round(distance * 1200 / (PI * wheel_diameter));
	cout << "desirePos: " << desirePos << endl;
	//current encoder counts, which is average of left and right
	int currentPos = 0;
	//Position error
	int err = desirePos;
	char wkey = NULL;
	while (err>0)
	{
		//make sure no object is before the base
		ReadSornaSensor();
		
		if (((sornar[0] < 50 && sornar[0]>8) || (sornar[1] < 50 && sornar[1] > 8) || (sornar[2] < 50&&sornar[2]>8)) && direction == 1)
		{
			stop();
			if(!PassObstacle(1)) break;
		}
		else if (sornar[3] < 50 && direction == 0)
		{
			stop();
			if(!PassObstacle(0)) break;
			
		}
		driveWithSpeed(vl, vr);
		Sleep(50);
		getEncoderCounts();
		currentPos = (abs(e[0]) + abs(e[1])) / 2;
		cout << "current Pos.: " << currentPos << endl;
		err = desirePos - currentPos;
		if (_kbhit()) wkey = _getch();
		if (wkey == 27)
		{
			break;
		}
	}
	stop();
	return;
}


void BaseControl::WalkRobotBySpeedLeftTick(float left, float right, int tick)
{
	float vl=left;
	float vr=right;
	resetEncoders();
	//target encoder counts
	int desiretick = tick;
	cout << "remainning tick: " << desiretick << endl;
	//current encoder counts, which is average of left and right
	int currenttick = 0;
	//Position error
	int err = desiretick;
	char wkey = NULL;
	while (err>0)
	{
		//make sure no object is before the base
		driveWithSpeed(vl, vr);
		Sleep(50);
		getEncoderCounts();
		currenttick = abs(e[0]);
		cout << "current tick.: " << currenttick << endl;
		err = desiretick- currenttick;
		if (_kbhit()) wkey = _getch();
		if (wkey == 27)
		{
			break;
		}
	}
	stop();
	return;
}
//rotate robot with a certain angle
//the angular speed is set at 1 rad/s

void BaseControl::RotateRobot(double angle, int direction)
{
	float vl;
	float vr;
	//rotate in clockwise direction
	if (direction == 1)
	{
		vl = A2L;
		vr = -A2L;
	}
	//rotate in counterclockwise direction
	else if (direction == 0)   
	{
		vl = -A2L;
		vr = A2L;
	}
	else cout << "The rotating direction is unknown!!" << endl;
	resetEncoders();
    //target encoder counts
	int desirePos =(int) round( abs((abs(angle)-10)) * 10 * wheel_track / (3 * wheel_diameter));
	cout << "desirPos: " << desirePos << endl;
	//current encoder counts, which is average of left and right
	int currentPos=0;
	//Position error
	int err = desirePos;
	char rkey = NULL;
	while (err>0)
	{
		driveWithSpeed(vl, vr);
		Sleep(50);
		getEncoderCounts();
		currentPos = (abs(e[0]) + abs(e[1])) / 2;
		cout << "current Pos.: " << currentPos << endl;
		err = desirePos - currentPos;
		if (_kbhit()) rkey = _getch();
		if (rkey == 27)
		{
			break;
		}
	}
	stop();
	return;

}

//walk the robot in follow mode
void BaseControl::Follow()
{

}

//Circle the robot around human with certain degree
//the radius along the axis,so if need to circle around a point that not at this direction,must have a transformation first
void BaseControl::CircleRobot(double radius,double angle,int direction)
{
	float vl;
	float vr;
	//rotate in clockwise direction
	if (direction == 1)
	{
		vl = Walking_Speed - Walking_Speed *wheel_track / (2 * radius);
		vr = Walking_Speed + Walking_Speed *wheel_track / (2 * radius);
	}
	//rotate in counterclockwise direction
	else if (direction == 0)
	{
		vl = Walking_Speed + Walking_Speed *wheel_track/(2* radius);
		vr = Walking_Speed - Walking_Speed *wheel_track / (2 * radius);
	}
	else cout << "The rotating direction is unknown!!" << endl;
	resetEncoders();
	//target encoder counts
	int desirePos = (int)round(angle * 20 * radius / (3 * wheel_diameter));
	cout << "desirPos: " << desirePos << endl;
	//current encoder counts, which is average of left and right
	int currentPos = 0;
	//Position error
	int err = desirePos;
	while (err>0)
	{
		//make sure no object is before the base
		ReadSornaSensor();
		if (((sornar[0] < 40 && sornar[0]>8) || (sornar[1] < 40 && sornar[1] > 8) || (sornar[2] < 40 && sornar[2]>8)) && direction == 1)
		{
			stop();
			PassObstacle(1);
		}
		else if (sornar[3] < 50 && direction == 0)
		{
			stop();
			PassObstacle(0);
		}
		driveWithSpeed(vl, vr);
		Sleep(50);
		getEncoderCounts();
		currentPos = (abs(e[0]) + abs(e[1])) / 2;
		cout << "current Pos.: " << currentPos << endl;
		err = desirePos - currentPos;
	}
	stop();
	return;
}

//Pass an obstacle
bool BaseControl::PassObstacle(int direction)
{
	stop();
	ReadSornaSensor();
	char pkey = NULL;
	//if Obstacle almost contact with the base
	while (sornar[0] < 4 || sornar[1] < 4 || sornar[2] < 4 || sornar[3] < 4)
	{
		RotateRobot(15, 1);
		ReadSornaSensor();
		Sleep(900);
		if (_kbhit()) pkey = _getch();
		if (pkey == 27)
		{
			return false;
		}
	}
	//Obstacle is in front of the mobile base
	
	while (((sornar[0] < 40 &&sornar[0]>8) || (sornar[1] < 40&&sornar[1]>8 ) || (sornar[2] < 40 && sornar[2]>8))&&direction==1 )
	{
		
		if ( sornar[0] < sornar[2])
			RotateRobot(25, 1);
		else if ( sornar[0] > sornar[2])
			RotateRobot(25, 0);
		else RotateRobot(80, 1);
		ReadSornaSensor();
		Sleep(900);
		if (_kbhit()) pkey = _getch();
		if (pkey == 27)
		{
			return false;
		}
	}
	//Obstacle is behind the mobile base
	while (sornar[3] < 40 && direction == 0)
	{
		
		 RotateRobot(20, 0);
		ReadSornaSensor();
		Sleep(900);
		if (_kbhit()) pkey = _getch();
		if (pkey == 27)
		{
			return false;
		}
	}
	return true;
}


void BaseControl::OneKeyTest(float distance, int direction)
{
	int state = 3;
	char pkey = NULL;

	while (true)
	{
		ReadSornaSensor();
		int i = 0;
		while (i <= 2){
			cout << "Sornar sensor reads:" << sornar[i] << endl;
			i++;
		}
		if ((sornar[0] <= sornar[2]) && (sornar[0] < 40))
		{
			state = 0;//obstacle at left front or directly in the front
		}
		else if ((sornar[2] < sornar[0]) && (sornar[2] < 40))
		{
			state = 1;//obstacle at right front
		}
		else if (sornar[1] < 40){
			state = 2;//no obstacle
		}
		else{
			state = 3;
			
		}
		cout << state<<endl;
	
		//if (_kbhit()) pkey = _getch();
		//if (pkey == 0)
		//{
		//	break;
		//}
		if (state == 3)
		{
			WalkRobot(distance, direction);
		}
		else
		{
			PassObstacle(direction);
			WalkRobot(distance, direction);
			if (state == 0)
			{
				RotateRobot(45, 0);
			}
			else
			{
				RotateRobot(45, 1);
			}
		}
	}
	return;
}
//test comment
double BaseControl::rotateX(float x,float y,double a)
{
//	float tx=x;
//	float ty=y;
	return cos(a)*x - sin(a)*y;
}
double BaseControl::rotateY(float x,float y,double a)
{
//	float tx=x;
//	float ty=y;
	return sin(a)*x + cos(a)*y;
}
void BaseControl::GoToXY(float x,float y)
{
	float cx=x*100;
	float cy=y*100;
	float tx,ty=0;
	float longside=sqrt(x*x+y*y);
	float cost=x/longside;
	float sint=y/longside;
	double angle=0;
	int LeftRightFlag=0;
	while ((abs(cy)>10)&&(abs(cx)>10))
	{
		longside=sqrt(cx*cx+cy*cy);
		cost=cx/longside;
		sint=-cy/longside;
		
		ReadSornaSensor();
		angle=atan2(cy,cx)/M_PI*180;
		RotateRobot(angle,cy<=0);
		tx=cx;
		ty=cy;
		cx=cost*tx-sint*ty;
		cy=sint*tx+cost*ty;
		ReadSornaSensor();
		while (!(sornar[0]>4 && sornar[1]>4 && sornar[2]>4 ) )
		{
			if(sornar[0]>sornar[1])
			{
				LeftRightFlag=-1;
			}
			else
			{
				LeftRightFlag=1;
			}
			RotateRobot(30,LeftRightFlag==1);
			tx=cx;
			ty=cy;
			cx=rotateX(tx,ty,30*-LeftRightFlag);
			cy=rotateY(tx,ty,30*-LeftRightFlag);
		}
		WalkRobot(0.15,1);
		cx=cx-15;
		cout<<cx<<endl;
		cout<<cy<<endl;
	}
	return;
}
void BaseControl::GoToXYwithoutRotation(float xx,float yy)
{
//	driveWithSpeed(float left, float right)
//	max=70
	float x=xx*100;
	float y=yy*100;
	float WheelDiam=12.5;
	float WheelTrack=34;
	float b=0;
	float s=0;
	float c=0;
	float vl,vr,va=0;
	float speedtick=100;
	float tickpercm=31.8310;
	float direangle,moveangle=0;
	int movetick=0;
	if (y!=0)
	{
		b=y/2+x*x/(2*y);
		s=-x/y;
		c=abs(b);
	}
	else
	{
		c=0;
	}
	cout<<"c is";
	cout<<c<<endl;
	if(c==0)
	{
//		if()
		WalkRobot(x,1);
	}
	else
	{
		va=2*M_PI/speedtick;
		if(y>0)
		{
			vl=va*(abs(c)-WheelTrack/2);
			vr=va*(abs(c)+WheelTrack/2);
		}
		else
		{
			vl=va*(abs(c)+WheelTrack/2);
			vr=va*(abs(c)-WheelTrack/2);
		}
		direangle=atan2(y,x);
		moveangle=2*abs(direangle);
		movetick=(int) moveangle*abs(c)/tickpercm;
		cout<<vl<<endl;
		cout<<vr<<endl;
		WalkRobotBySpeedLeftTick(vl/100, vr/100, movetick);
	}
	return;
}
