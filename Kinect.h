#pragma once
#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include "Basecontrol.h"
#include <Kinect.h>



using namespace cv;
// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

class CBodyBasics
{
	//friend class CBodyRepair;
	//kinect 2.0 的深度空间的高*宽是 424 * 512，在官网上有说明
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	static const int        cColorWith = 1920;
	static const int        cColorHeight = 1080;


public:
	CBodyBasics();
	~CBodyBasics();

	BaseControl             EAI;

	void                    Update();//获得骨架、背景二值图和深度信息
	HRESULT                 InitializeDefaultSensor();//用于初始化kinect
	//显示图像的Mat
	cv::Mat skeletonImg;
	cv::Mat depthImg;
	

	// store the lenth of each bone
	float static Length_Neck_SpineShoulder;
	float static Length_SpineShoulder_SpineMid;
	float static Length_SpineMid_SpineBase;
	float static Length_SpineShoulder_Shoulder;
	float static Length_SpineBase_Hip;
	float static Length_Shoulder_Elbow;
	float static Length_Elbow_Wrist;
	float static Length_Wrist_Hand;
	float static Length_Hand_Handtip;
	float static Length_Wrist_Thumb;
	float static Length_Hip_Knee;
	float static Length_Knee_Ankle;
	float static Length_Ankle_Foot;
	//tracking human body
	void CBodyBasics::TrackingHuman();
	
	void CBodyBasics::GoHuman();
	// Track the state of human
	//the state of human is divided to walking and still
	bool CBodyBasics::TrackStateofHuman();
	//Track the hand of human body
	bool CBodyBasics::TrackHands();

private:
	IKinectSensor*          m_pKinectSensor;//kinect源
	IBodyFrameReader*       m_pBodyFrameReader;//用于骨架数据读取
	IDepthFrameReader*      m_pDepthFrameReader;//用于深度数据读取
	IBodyIndexFrameReader*  m_pBodyIndexFrameReader;//用于背景二值图读取
	IColorFrameReader*      m_pColorFrameReader;// use to read color map
	ICoordinateMapper*      m_pCoordinateMapper;//用于坐标变换
	
	
	Joint                   JspineBase;//record key joints
	Joint                   JhandLeft;
	Joint                   JhandRight;
	//record the position of spine_base and hands
	
	cv::Point2f spineBase_D = (0, 0);
	cv::Point2f handLeft_D = (0, 0);
	cv::Point2f handRight_D = (0, 0);
	//通过获得到的信息，把骨架和背景二值图画出来
	void DrawBody(int nBodyCount, IBody** ppBodies);
	//画骨架函数
	void DrawBone(const Joint* pJoints, const DepthSpacePoint* depthSpacePosition, JointType joint0, JointType joint1);
	//画手的状态函数
	void DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState);
	//tracking the state of two neighbour joints, if the two joints are both tracked, then calculate the lenght of bone consited by the two joints
	float TrackingJointState(const Joint* pJoints, JointType joint0, JointType joint1);
	// Initialize the length of each bone
	void InitializeSkeleton(int nBodyCount, IBody** ppBodies);
	//calculate the distance from camera to joint
	double CBodyBasics::Distance(Joint joint);
	//calculate the location angle of joint reference to camera
	double CBodyBasics::Angle(Joint joint);
	
	

};


