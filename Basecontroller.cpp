// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SerialClass.h"
#include "Basecontrol.h"
#include "Kinect.h"


using namespace std;

//bool communication(char *cmd, char *comPort);

int main(int argc, CHAR* argv[])
{
	char comPort[] = "COM3";
	char cmd[] = "David,nihao!";
	
	/*BaseControl EAI;
	
	EAI.arduino = new Serial(EAI.comPort);*/
	//while (1){
	//	/*if (EAI.sendCommand(cmd, arduinoR) )
	//		cout << "Commands have been sent correctly!" << endl;
	//	else cout << "There are some errs in sending data!" << endl;
	//	Sleep(1000);
	//	
	//	if (EAI.readingData(25,arduinoR)) 
	//		cout << "OK"<< endl;
	//	else cout << "There are some errs in reading data!" << endl;*/
	//	EAI.getBaudrate();
	//	EAI.getEncoderCounts();
	//	EAI.ReadSornaSensor();
	//	/*EAI.resetEncoders();*/
	//	/*EAI.driveWithSpeed(0.1, 0.1);
	//	EAI.WalkRobot(1.2, 1);
	//	EAI.RotateRobot(30, 0);
	//	EAI.CircleRobot(0.6, 90,1);*/
	//	Sleep(10000);

	//}
	
	CBodyBasics myKinect;
	BaseControl MEAI;
	//HRESULT hr = myKinect.InitializeDefaultSensor();
	HRESULT hr = 1;
	if (SUCCEEDED(hr))
	{
		char key=NULL;
		while (1)
		{
			if (_kbhit()) key = _getch();
		   	//manual mode
			if (key == 'm')
			{
				MEAI.arduino = new Serial(MEAI.comPort);
				MEAI.getBaudrate();
				double vel = 0.2;
				double vel_R = 0.1;
				cout << "vel: " << vel << " " << "vel_R: "<<vel_R << endl;
				while (key != 27)
				{
					key = NULL;
					if (_kbhit()) key = _getch();
					//myKinect.Update();
					//cout << "updated!!" << endl;
					switch (key)
					{
					case 48: MEAI.driveWithSpeed(0, 0); break;     // press 0
					case 72: MEAI.driveWithSpeed(vel, vel); break;  //up
					case 80: MEAI.driveWithSpeed(-vel, -vel); break;  //down
					case 75: MEAI.driveWithSpeed(vel_R, -vel_R); break;  //left
					case 77: MEAI.driveWithSpeed(-vel_R, vel_R); break;  //right
					case 43: vel = 1.1*vel; vel_R = 1.1*vel_R; //"+',increase speed 10%
						     cout << "vel: " << vel << " " << "vel_R: " << vel_R << endl; 
							 break;  
					case 45: vel = 0.9*vel; vel_R = 0.9*vel_R; //'-',decrease speed by 10%
						     cout << "vel: " << vel << " " << "vel_R: " << vel_R << endl;
						     break;  
					case 't':MEAI.OneKeyTest(0.3,1); 
						     break;
					case 'b':MEAI.GoToXY(30,30); 
						     break;
					case 'c':MEAI.GoToXYwithoutRotation(30,30); 
						     break;
					default:
						break;
					}
				}
				MEAI.arduino->~Serial();
			}
			//Automode
			//if (key=='a')
			//{
			//	myKinect.EAI.arduino = new Serial(myKinect.EAI.comPort);
			//	while (key!=27)
			//	{ 
			//		key = NULL;
			//		myKinect.Update();
			//		if (_kbhit()) key = _getch();
			//	    myKinect.TrackingHuman();
			//	    Sleep(500);
			//	    if (myKinect.TrackStateofHuman())
			//	    {
			//		  cout << "The person is still walking! Please keep tracking!" << endl;
			//		  continue;
			//	    }
			//	    else
			//	    {
			//		  Sleep(100);
			//		  if (myKinect.TrackStateofHuman())
			//		  {
			//			cout << "The person is still walking! Please keep tracking!" << endl;
			//			continue;
			//		  }
			//		  else cout << "The person is still! Please observe carefully!" << endl;
			//	    }


			//	    if (myKinect.TrackHands())
			//	    {
			//		  cout << "The hands are all observed!" << endl;
			//		  cout << "Please start recognition program!" << endl;
			//		  char keypressed;
			//		  //cin >> keypressed;
			//		  //while (keypressed != 'c') break;

			//	    }
			//	    else cout << "Can't observe the hands of this person. Please circle the robot around him!" << endl;

			//	    //myRepair.JointOrientationCal();
			//	    //i++;
			//	   //ImageSave(myKinect.skeletonImg, i);
			//	   if (_kbhit()) key = _getch();
			//   }
			//	myKinect.EAI.arduino->~Serial();
		 //  }
		if (key == 27) break;
		}
	}
	else
	{
		cout << "kinect initialization failed!" << endl;
		system("pause");
	}
	
	
	Sleep(5000);
	return 0;
	
}

