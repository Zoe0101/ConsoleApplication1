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
	//kinect 2.0 ����ȿռ�ĸ�*���� 424 * 512���ڹ�������˵��
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	static const int        cColorWith = 1920;
	static const int        cColorHeight = 1080;


public:
	CBodyBasics();
	~CBodyBasics();

	BaseControl             EAI;

	void                    Update();//��ùǼܡ�������ֵͼ�������Ϣ
	HRESULT                 InitializeDefaultSensor();//���ڳ�ʼ��kinect
	//��ʾͼ���Mat
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
	IKinectSensor*          m_pKinectSensor;//kinectԴ
	IBodyFrameReader*       m_pBodyFrameReader;//���ڹǼ����ݶ�ȡ
	IDepthFrameReader*      m_pDepthFrameReader;//����������ݶ�ȡ
	IBodyIndexFrameReader*  m_pBodyIndexFrameReader;//���ڱ�����ֵͼ��ȡ
	IColorFrameReader*      m_pColorFrameReader;// use to read color map
	ICoordinateMapper*      m_pCoordinateMapper;//��������任
	
	
	Joint                   JspineBase;//record key joints
	Joint                   JhandLeft;
	Joint                   JhandRight;
	//record the position of spine_base and hands
	
	cv::Point2f spineBase_D = (0, 0);
	cv::Point2f handLeft_D = (0, 0);
	cv::Point2f handRight_D = (0, 0);
	//ͨ����õ�����Ϣ���ѹǼܺͱ�����ֵͼ������
	void DrawBody(int nBodyCount, IBody** ppBodies);
	//���Ǽܺ���
	void DrawBone(const Joint* pJoints, const DepthSpacePoint* depthSpacePosition, JointType joint0, JointType joint1);
	//���ֵ�״̬����
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


