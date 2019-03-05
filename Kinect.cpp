#include "stdafx.h"
#include "Kinect.h"
#include <iostream>

//#include "Microsoft.Kinect.Tools.dll"

using namespace std;
using namespace cv;


/// Initializes the default Kinect sensor
HRESULT CBodyBasics::InitializeDefaultSensor()
{
	//�����ж�ÿ�ζ�ȡ�����ĳɹ����
	HRESULT hr;

	//����kinect
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr)){
		return hr;
	}

	//�ҵ�kinect�豸
	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;//��ȡ�Ǽ�
		IDepthFrameSource* pDepthFrameSource = NULL;//��ȡ�����Ϣ
		IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//��ȡ������ֵͼ
		IColorFrameSource* pColorFrameSource = NULL;//read RGB image

		//��kinect
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

	//skeletonImg,���ڻ��Ǽܡ�������ֵͼ��MAT
	skeletonImg.create(cDepthHeight, cDepthWidth, CV_8UC3);
	skeletonImg.setTo(0);

	//depthImg,���ڻ������Ϣ��MAT
	depthImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthImg.setTo(0);

	return hr;
}


/// Main processing function
void CBodyBasics::Update()
{
	//ÿ�������skeletonImg
	skeletonImg.setTo(0);

	//�����ʧ��kinect���򲻼�������
	if (!m_pBodyFrameReader)
	{
		
		return;
	}

	IBodyFrame* pBodyFrame = NULL;//�Ǽ���Ϣ
	IDepthFrame* pDepthFrame = NULL;//�����Ϣ
	IBodyIndexFrame* pBodyIndexFrame = NULL;//������ֵͼ

	//��¼ÿ�β����ĳɹ����
	HRESULT hr = S_OK;

	//---------------------------------------��ȡ������ֵͼ����ʾ---------------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);//��ñ�����ֵͼ��Ϣ

	}
	
	if (SUCCEEDED(hr)){
		BYTE *bodyIndexArray = new BYTE[cDepthHeight * cDepthWidth];//������ֵͼ��8 bit uchar�������Ǻ�ɫ��û���ǰ�ɫ
		pBodyIndexFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, bodyIndexArray);

		//�ѱ�����ֵͼ����MAT��
		uchar* skeletonData = (uchar*)skeletonImg.data;
		for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
			*skeletonData = ~bodyIndexArray[j]; ++skeletonData;
			*skeletonData = ~bodyIndexArray[j]; ++skeletonData;
			*skeletonData = ~bodyIndexArray[j]; ++skeletonData;
		}
		delete[] bodyIndexArray;
	}
	
	SafeRelease(pBodyIndexFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����

	//-----------------------��ȡ������ݲ���ʾ--------------------------
	//if (SUCCEEDED(hr)){
	//	hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);//����������
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
	//		UINT16 *depthArray = new UINT16[cDepthHeight * cDepthWidth];//���������16λunsigned int
	//		pDepthFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, depthArray);

	//		//��������ݻ���MAT��
	//		uchar* depthData = (uchar*)depthImg.data;
	//		for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
	//			*depthData = ((depthArray[j] >= nDepthMinReliableDistance) && (depthArray[j] <= nDepthMaxDistance) ? (depthArray[j] % 256) : 0);
	//			//*depthData = depthArray[j];
	//			++depthData;
	//		}
	//		delete[] depthArray;
	//	}
	//}
	//SafeRelease(pDepthFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����
	//imshow("depthImg", depthImg);
	cv::waitKey(5);
	//-----------------------------��ȡ�Ǽܲ���ʾ----------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);//��ȡ�Ǽ���Ϣ
	}
	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[BODY_COUNT] = { 0 };//ÿһ��IBody����׷��һ���ˣ��ܹ�����׷��������

		if (SUCCEEDED(hr))
		{
			//��kinect׷�ٵ����˵���Ϣ���ֱ�浽ÿһ��IBody��
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}


		//if (SUCCEEDED(hr))
		//{
		//	//��ÿһ��IBody��initialize their bones' length
		//	InitializeSkeleton(BODY_COUNT, ppBodies);
		//}
		if (SUCCEEDED(hr))
		{
			//��ÿһ��IBody�������ҵ����ĹǼ���Ϣ�����һ�����
			DrawBody(BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);//�ͷ�����
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
	SafeRelease(pBodyFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����

}

/// Handle new body data
void CBodyBasics::DrawBody(int nBodyCount, IBody** ppBodies)
{
	//��¼��������Ƿ�ɹ�
	HRESULT hr;

	//����ÿһ��IBody
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);

			if (SUCCEEDED(hr) && bTracked)
			{
				Joint joints[JointType_Count];//�洢�ؽڵ���
				HandState leftHandState = HandState_Unknown;//����״̬
				HandState rightHandState = HandState_Unknown;//����״̬

				//��ȡ������״̬
				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				//�洢�������ϵ�еĹؽڵ�λ��
				DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];

				//��ùؽڵ���
				hr = pBody->GetJoints(_countof(joints), joints);
				JspineBase = joints[JointType_SpineBase];//record the data of joint SpineBase
				JhandLeft = joints[JointType_HandLeft];
				JhandRight = joints[JointType_HandRight];
				if (SUCCEEDED(hr))
				{
					for (int j = 0; j < _countof(joints); ++j)
					{
						//���ؽڵ���������������ϵ��-1~1��ת���������ϵ��424*512��
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




//���ֵ�״̬
void CBodyBasics::DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState)
{
	//����ͬ�����Ʒ��䲻ͬ��ɫ
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
	default://���û��ȷ�������ƣ��Ͳ�Ҫ��
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
		//tracked skeleton��use green line
		line(skeletonImg, p1, p2, cvScalar(0, 255, 0), 4);
	}
	else
	{
		//inferred skeleton��use red line
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
			//circle robot around human counter clockwise with 190��-anglel-angler
			cout << "circle robot around human counter clockwise with "<<90+anglel+angler<<"�� ��" << endl;
			// Circle(linearspeed, rotatespeed);
			Update();
			while (JhandLeft.TrackingState != TrackingState_Tracked || JhandLeft.TrackingState != TrackingState_Tracked)
			{
				char mykey=NULL;
				//circle robot around human counter clockwise with 5��
				cout << "circle robot around human counter clockwise with 5�� !" << endl;
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
			//circle robot around human clockwise with 190��-anglel-angler
			cout << "circle robot around human clockwise with " << 90 + anglel + angler << "�� ��" << endl;
			// Circle(linearspeed, rotatespeed);
			Update();
			while (JhandLeft.TrackingState != TrackingState_Tracked || JhandLeft.TrackingState != TrackingState_Tracked)
			{
				char mykey = NULL;
				//circle robot around human clockwise with 5��
				cout << "circle robot around human clockwise with 5�� !" << endl;
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