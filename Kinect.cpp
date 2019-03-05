#include "stdafx.h"
#include "Kinect.h"
#include <iostream>

//#include "Microsoft.Kinect.Tools.dll"

using namespace std;
using namespace cv;


/// Initializes the default Kinect sensor
HRESULT CBodyBasics::InitializeDefaultSensor()
{
	//用于判断每次读取操作的成功与否
	HRESULT hr;

	//搜索kinect
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr)){
		return hr;
	}

	//找到kinect设备
	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;//读取骨架
		IDepthFrameSource* pDepthFrameSource = NULL;//读取深度信息
		IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//读取背景二值图
		IColorFrameSource* pColorFrameSource = NULL;//read RGB image

		//打开kinect
		hr = m_pKinectSensor->Open();

		//coordinatemapper
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		//bodyframe
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		//depth frame
		if (SUCCEEDED(hr)){
			hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		}

		if (SUCCEEDED(hr)){
			hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		}

		//body index frame
		if (SUCCEEDED(hr)){
			hr = m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		}

		if (SUCCEEDED(hr)){
			hr = pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);
		}
		// color frame
		if (SUCCEEDED(hr)){
			hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		}

		if (SUCCEEDED(hr)){
			hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
		}

		SafeRelease(pBodyFrameSource);
		SafeRelease(pDepthFrameSource);
		SafeRelease(pBodyIndexFrameSource);
		SafeRelease(pColorFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		std::cout << "Kinect initialization failed!" << std::endl;
		return E_FAIL;
	}

	//skeletonImg,用于画骨架、背景二值图的MAT
	skeletonImg.create(cDepthHeight, cDepthWidth, CV_8UC3);
	skeletonImg.setTo(0);

	//depthImg,用于画深度信息的MAT
	depthImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthImg.setTo(0);

	return hr;
}


/// Main processing function
void CBodyBasics::Update()
{
	//每次先清空skeletonImg
	skeletonImg.setTo(0);

	//如果丢失了kinect，则不继续操作
	if (!m_pBodyFrameReader)
	{
		
		return;
	}

	IBodyFrame* pBodyFrame = NULL;//骨架信息
	IDepthFrame* pDepthFrame = NULL;//深度信息
	IBodyIndexFrame* pBodyIndexFrame = NULL;//背景二值图

	//记录每次操作的成功与否
	HRESULT hr = S_OK;

	//---------------------------------------获取背景二值图并显示---------------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);//获得背景二值图信息

	}
	
	if (SUCCEEDED(hr)){
		BYTE *bodyIndexArray = new BYTE[cDepthHeight * cDepthWidth];//背景二值图是8 bit uchar，有人是黑色，没人是白色
		pBodyIndexFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, bodyIndexArray);

		//把背景二值图画到MAT里
		uchar* skeletonData = (uchar*)skeletonImg.data;
		for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
			*skeletonData = ~bodyIndexArray[j]; ++skeletonData;
			*skeletonData = ~bodyIndexArray[j]; ++skeletonData;
			*skeletonData = ~bodyIndexArray[j]; ++skeletonData;
		}
		delete[] bodyIndexArray;
	}
	
	SafeRelease(pBodyIndexFrame);//必须要释放，否则之后无法获得新的frame数据

	//-----------------------获取深度数据并显示--------------------------
	//if (SUCCEEDED(hr)){
	//	hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);//获得深度数据
	//	if (SUCCEEDED(hr)){
	//		USHORT nDepthMinReliableDistance = 0;
	//		USHORT nDepthMaxDistance = 0;
	//		IFrameDescription* pFrameDescription = NULL;
	//		if (SUCCEEDED(hr))
	//		{
	//			hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
	//		}
	//		if (SUCCEEDED(hr))
	//		{
	//			hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
	//		}
	//		if (SUCCEEDED(hr)){
	//			nDepthMaxDistance = USHRT_MAX;
	//		}
	//		UINT16 *depthArray = new UINT16[cDepthHeight * cDepthWidth];//深度数据是16位unsigned int
	//		pDepthFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, depthArray);

	//		//把深度数据画到MAT中
	//		uchar* depthData = (uchar*)depthImg.data;
	//		for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
	//			*depthData = ((depthArray[j] >= nDepthMinReliableDistance) && (depthArray[j] <= nDepthMaxDistance) ? (depthArray[j] % 256) : 0);
	//			//*depthData = depthArray[j];
	//			++depthData;
	//		}
	//		delete[] depthArray;
	//	}
	//}
	//SafeRelease(pDepthFrame);//必须要释放，否则之后无法获得新的frame数据
	//imshow("depthImg", depthImg);
	cv::waitKey(5);
	//-----------------------------获取骨架并显示----------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);//获取骨架信息
	}
	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[BODY_COUNT] = { 0 };//每一个IBody可以追踪一个人，总共可以追踪六个人

		if (SUCCEEDED(hr))
		{
			//把kinect追踪到的人的信息，分别存到每一个IBody中
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}


		//if (SUCCEEDED(hr))
		//{
		//	//对每一个IBody，initialize their bones' length
		//	InitializeSkeleton(BODY_COUNT, ppBodies);
		//}
		if (SUCCEEDED(hr))
		{
			//对每一个IBody，我们找到他的骨架信息，并且画出来
			DrawBody(BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);//释放所有
		}
	}
	else
	{
		JspineBase.TrackingState = TrackingState_NotTracked;//record the data of joint SpineBase
		JspineBase.Position.X = 0;
		//JspineBase.Position.Y = 0;
		//JspineBase.Position.Z = 0;
		JhandLeft.TrackingState = TrackingState_NotTracked;
		JhandRight.TrackingState = TrackingState_NotTracked;
		return;
	}
	SafeRelease(pBodyFrame);//必须要释放，否则之后无法获得新的frame数据

}

/// Handle new body data
void CBodyBasics::DrawBody(int nBodyCount, IBody** ppBodies)
{
	//记录操作结果是否成功
	HRESULT hr;

	//对于每一个IBody
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);

			if (SUCCEEDED(hr) && bTracked)
			{
				Joint joints[JointType_Count];//存储关节点类
				HandState leftHandState = HandState_Unknown;//左手状态
				HandState rightHandState = HandState_Unknown;//右手状态

				//获取左右手状态
				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				//存储深度坐标系中的关节点位置
				DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];

				//获得关节点类
				hr = pBody->GetJoints(_countof(joints), joints);
				JspineBase = joints[JointType_SpineBase];//record the data of joint SpineBase
				JhandLeft = joints[JointType_HandLeft];
				JhandRight = joints[JointType_HandRight];
				if (SUCCEEDED(hr))
				{
					for (int j = 0; j < _countof(joints); ++j)
					{
						//将关节点坐标从摄像机坐标系（-1~1）转到深度坐标系（424*512）
						m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthSpacePosition[j]);
					}

					//------------------------hand state left-------------------------------
					/*DrawHandState(depthSpacePosition[JointType_HandLeft], leftHandState);
					DrawHandState(depthSpacePosition[JointType_HandRight], rightHandState);*/

					//---------------------------body-------------------------------
					DrawBone(joints, depthSpacePosition, JointType_Head, JointType_Neck);
					DrawBone(joints, depthSpacePosition, JointType_Neck, JointType_SpineShoulder);
					DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_SpineMid);
					DrawBone(joints, depthSpacePosition, JointType_SpineMid, JointType_SpineBase);
					DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderRight);
					DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderLeft);
					DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipRight);
					DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipLeft);

					// -----------------------Right Arm ------------------------------------ 
					DrawBone(joints, depthSpacePosition, JointType_ShoulderRight, JointType_ElbowRight);
					DrawBone(joints, depthSpacePosition, JointType_ElbowRight, JointType_WristRight);
					DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_HandRight);
					DrawBone(joints, depthSpacePosition, JointType_HandRight, JointType_HandTipRight);
					DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_ThumbRight);

					//----------------------------------- Left Arm--------------------------
					DrawBone(joints, depthSpacePosition, JointType_ShoulderLeft, JointType_ElbowLeft);
					DrawBone(joints, depthSpacePosition, JointType_ElbowLeft, JointType_WristLeft);
					DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_HandLeft);
					DrawBone(joints, depthSpacePosition, JointType_HandLeft, JointType_HandTipLeft);
					DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_ThumbLeft);

					// ----------------------------------Right Leg--------------------------------
					DrawBone(joints, depthSpacePosition, JointType_HipRight, JointType_KneeRight);
					DrawBone(joints, depthSpacePosition, JointType_KneeRight, JointType_AnkleRight);
					DrawBone(joints, depthSpacePosition, JointType_AnkleRight, JointType_FootRight);

					// -----------------------------------Left Leg---------------------------------
					DrawBone(joints, depthSpacePosition, JointType_HipLeft, JointType_KneeLeft);
					DrawBone(joints, depthSpacePosition, JointType_KneeLeft, JointType_AnkleLeft);
					DrawBone(joints, depthSpacePosition, JointType_AnkleLeft, JointType_FootLeft);
					//---------------------------------draw joints---------------------------------------
					for (i = 0; i < JointType_Count; ++i)
					{
						if (joints[i].TrackingState == TrackingState_Inferred)
						{
							circle(skeletonImg, cvPoint(depthSpacePosition[i].X, depthSpacePosition[i].Y), 4, (255, 0, 0), 0.8);
						}
						else if (joints[i].TrackingState == TrackingState_Tracked)
						{
							circle(skeletonImg, cvPoint(depthSpacePosition[i].X, depthSpacePosition[i].Y), 4, (0, 255, 0), 1.5);
						}
					}
					
					
					//give the position of Spine_Base and hands to vector p0,p1 and p2 in Depth Space
					spineBase_D = Point2f(depthSpacePosition[JointType_SpineBase].X, depthSpacePosition[JointType_SpineBase].Y);
					handLeft_D = Point2f(depthSpacePosition[JointType_HandLeft].X, depthSpacePosition[JointType_HandLeft].Y);
					handRight_D = Point2f(depthSpacePosition[JointType_HandRight].X, depthSpacePosition[JointType_HandRight].Y);
					//circle out the spine_Base joint and two hands
					//circle(skeletonImg, spineBase_D, 16, (0, 0, 255), 4);
					circle(skeletonImg, handLeft_D, 16, (0, 0, 255), 4);
					circle(skeletonImg, handRight_D, 16, (0, 0, 255), 4);
				}
				delete[] depthSpacePosition;
			}
		}
	}
	cv::imshow("skeletonImg", skeletonImg);
	cv::waitKey(5);
}




//画手的状态
void CBodyBasics::DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState)
{
	//给不同的手势分配不同颜色
	CvScalar color;
	switch (handState){
	case HandState_Open:
		color = cvScalar(255, 0, 0);
		break;
	case HandState_Closed:
		color = cvScalar(0, 255, 0);
		break;
	case HandState_Lasso:
		color = cvScalar(0, 0, 255);
		break;
	default://如果没有确定的手势，就不要画
		return;
	}

	circle(skeletonImg,
		cvPoint(depthSpacePosition.X, depthSpacePosition.Y),
		20, color, -1);
}


/// Draws one bone of a body (joint to joint)
void CBodyBasics::DrawBone(const Joint* pJoints, const DepthSpacePoint* depthSpacePosition, JointType joint0, JointType joint1)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;
	TrackingState joint1State = pJoints[joint1].TrackingState;

	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	// Don't draw if both points are inferred
	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	CvPoint p1 = cvPoint(depthSpacePosition[joint0].X, depthSpacePosition[joint0].Y),
		p2 = cvPoint(depthSpacePosition[joint1].X, depthSpacePosition[joint1].Y);

	// We assume all drawn bones are inferred unless BOTH joints are tracked
	if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
	{
		//tracked skeleton，use green line
		line(skeletonImg, p1, p2, cvScalar(0, 255, 0), 4);
	}
	else
	{
		//inferred skeleton，use red line
		line(skeletonImg, p1, p2, cvScalar(0, 0, 255), 1);
	}
}


/// Constructor
CBodyBasics::CBodyBasics() :
m_pKinectSensor(NULL),
m_pCoordinateMapper(NULL),
m_pBodyFrameReader(NULL){}

/// Destructor
CBodyBasics::~CBodyBasics()
{
	SafeRelease(m_pBodyFrameReader);
	SafeRelease(m_pCoordinateMapper);

	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}






//tracking human body
//the output is distance from the tracked human and his/her orientation refer to the robot
//parameters:
void CBodyBasics::TrackingHuman()
{   
	char mykey = NULL;
	//if nobody in the view, then rotate the robot and try to find human arround
	while(JspineBase.TrackingState == TrackingState_NotTracked)
	{
		cout << "no human is found!! Please look around!" << endl;
		Sleep(3000);
		EAI.RotateRobot(5,1);
		 
		 int i = 0;
		 for (i = 0; i < 20; i++)
		 {
			 Update();	
			 if (JspineBase.TrackingState != TrackingState_NotTracked) break;
		 }
		 if (JspineBase.TrackingState != TrackingState_NotTracked) break;
		 if (_kbhit()) mykey = _getch();
		 if (mykey == 27)  break;
		//Sleep(2500);
	}
	//if at least a human is tracked
	
	cout << "the people is at " << Distance(JspineBase) << "meters away!" << endl;
	if (Angle(JspineBase) < 0) cout << "the people is at " << Angle(JspineBase) << "degree in the left!" << endl;
	else cout << "the people is at " << Angle(JspineBase) << " degree in the right!" << endl;
	
	//judging if the robot at a proper orientation to observe the human
	while (Angle(JspineBase) > 20 || Angle(JspineBase) < -20)
	{
		
		//adjust the view point of Robot
		if (Angle(JspineBase) < -20)
		{
			cout << "the people is at " << Angle(JspineBase) << " degree in the right!" << endl;
			EAI.RotateRobot(abs(Angle(JspineBase))-5,1);
			//Sleep(100);
		}
		else
		{
			EAI.RotateRobot(abs(Angle(JspineBase))-5,0);
			//Sleep(100);
			cout << "the people is at " << Angle(JspineBase) << " degree in the left!" << endl;
		}
		cout << "Rotate robot with " << Angle(JspineBase) << " degree!" << endl;

		
		Update();
		if (JspineBase.TrackingState == TrackingState_NotTracked) break;
		if (_kbhit()) mykey = _getch();
		if (mykey == 27)  break;
		Sleep(100);
	}
	//the best distance is between 1.5~3.5m
	if (Distance(JspineBase) > 3.5 || Distance(JspineBase) < 1.5)
	{ 
		
		while (Distance(JspineBase) > 3.5 || Distance(JspineBase) < 1.5)
		{
			
			if (Distance(JspineBase) > 3.5)
			{
				EAI.WalkRobot(Distance(JspineBase) - 3, 1);
				//EAI.WalkRobot(0.5, 1);
				cout << "It's too far away! Please walk nearer!" << endl;
			}
			else
			{
				EAI.WalkRobot(2-Distance(JspineBase), 0);
				//EAI.WalkRobot(0.5, 0);
				cout << "It's too near! Be careful!" << endl;
			}
			
			Update();
			Update();
			cout << "The current distance is " << Distance(JspineBase) << " !" << endl;
			if (JspineBase.TrackingState == TrackingState_NotTracked) break;
			if (_kbhit()) mykey = _getch();
			if (mykey == 27)  break;
			Sleep(1000);
		}	

	}
	
	return;
}

void CBodyBasics::GoHuman()
{
	char mykey = NULL;
	float x,y=0;
	//if nobody in the view, then rotate the robot and try to find human arround
	while(JspineBase.TrackingState == TrackingState_NotTracked)
	{
		cout << "no human is found!! Please look around!" << endl;
		Sleep(3000);
		EAI.RotateRobot(5,1);
		 
		 int i = 0;
		 for (i = 0; i < 20; i++)
		 {
			 Update();	
			 if (JspineBase.TrackingState != TrackingState_NotTracked) break;
		 }
		 if (JspineBase.TrackingState != TrackingState_NotTracked) break;
		 if (_kbhit()) mykey = _getch();
		 if (mykey == 27)  break;
		//Sleep(2500);
	}
	//if at least a human is tracked
	
	cout << "the people is at " << Distance(JspineBase) << "meters away!" << endl;
	if (Angle(JspineBase) < 0) cout << "the people is at " << Angle(JspineBase) << "degree in the left!" << endl;
	else cout << "the people is at " << Angle(JspineBase) << " degree in the right!" << endl;
	x=Distance(JspineBase)*cos(Angle(JspineBase)/180*PI);
	if (Angle(JspineBase) < 0)
	{
		y=Distance(JspineBase)*sin(Angle(JspineBase)/180*PI);	
	}
	else
	{
		y=-Distance(JspineBase)*sin(Angle(JspineBase)/180*PI);
	}
	EAI.GoToXYwithoutRotation(x,y);
}
//Track the state of human
//the state of human is divided to walking and still
bool CBodyBasics::TrackStateofHuman()
{
	bool hr;
	//if hr=false, human body is treat as still
	//else the robot will think human is walking
	double distance = Distance(JspineBase);
	//cout << "the former distance is " << distance << "JspineBase" << JspineBase.Position.X << "," << JspineBase.Position.Y << "," << JspineBase.Position.Z<<endl;
	//Record the change of distance
	double DeltaD=0;
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		Update();
		//cout << "JspineBase" << JspineBase.Position.X << "," << JspineBase.Position.Y << "," << JspineBase.Position.Z << endl;
		
	}
	
	DeltaD = Distance(JspineBase) - distance;
	cout << "the current distance is " << Distance(JspineBase) << endl;
	cout << "the change of position is "<<DeltaD <<"meters"<< endl;
	if (DeltaD < 0.2) hr = false;//the nomal walking speed of human is 1m/s
	else hr = true;
	return hr;
	
}

//Track the hand of human body
bool CBodyBasics::TrackHands()
{
	bool hr;
	//judge the angle and distance again
	TrackingHuman();
	if (JhandLeft.TrackingState != TrackingState_Tracked || JhandLeft.TrackingState != TrackingState_Tracked)
	{
		double anglel = Angle(JhandLeft);
		double angler = Angle(JhandRight);
		if (anglel + angler < 0)
		{
			//circle robot around human counter clockwise with 190°-anglel-angler
			cout << "circle robot around human counter clockwise with "<<90+anglel+angler<<"° ！" << endl;
			// Circle(linearspeed, rotatespeed);
			Update();
			while (JhandLeft.TrackingState != TrackingState_Tracked || JhandLeft.TrackingState != TrackingState_Tracked)
			{
				char mykey=NULL;
				//circle robot around human counter clockwise with 5°
				cout << "circle robot around human counter clockwise with 5° !" << endl;
				// Circle(linearspeed, rotatespeed);
				Update();
				if (JspineBase.TrackingState == TrackingState_NotTracked) break;
				if (_kbhit()) mykey = _getch();
				if (mykey == 27)  break;
			}
			hr = true;
		}
		else
		{
			//circle robot around human clockwise with 190°-anglel-angler
			cout << "circle robot around human clockwise with " << 90 + anglel + angler << "° ！" << endl;
			// Circle(linearspeed, rotatespeed);
			Update();
			while (JhandLeft.TrackingState != TrackingState_Tracked || JhandLeft.TrackingState != TrackingState_Tracked)
			{
				char mykey = NULL;
				//circle robot around human clockwise with 5°
				cout << "circle robot around human clockwise with 5° !" << endl;
				// Circle(linearspeed, rotatespeed);
				Update();
				if (JspineBase.TrackingState == TrackingState_NotTracked) break;
				if (_kbhit()) mykey = _getch();
				if (mykey == 27)  break;
			}
			hr = true;
		}

	}
	else hr = true;
	return hr;

}

//calculate the distance from camera to joint
double CBodyBasics::Distance(Joint joint)
{
	cv::Point3f Jpoint = Point3f(joint.Position.X, joint.Position.Y, joint.Position.Z);
	return norm(Jpoint);
}
//calculate the location angle of joint reference to camera
double CBodyBasics::Angle(Joint joint)
{
	cv::Point3f Jpoint = Point3f(joint.Position.X, joint.Position.Y, joint.Position.Z);
	double angle = atan(Jpoint.x / Jpoint.z) * 180 / PI;//unit: dgree
	return angle;
}