#pragma once
#include <osgViewer/Viewer>
#include <osg/LineSegment>
#include <osg/Point>
#include <osg/Geometry>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osgGA/CameraManipulator>
#include <osgUtil/IntersectionVisitor>
#include <vector>
class TravelManipulator:public osgGA::CameraManipulator
{
public:
	TravelManipulator();
	~TravelManipulator();
	//�����μ��뵽����֮��
	static TravelManipulator* TravelToScene(osg::ref_ptr<osgViewer::Viewer> viewer);
	//���þ���
	virtual void setByMatrix(const osg::Matrixd& matrix);
	//���������
	virtual void setByInverseMatrix(const osg::Matrixd& matrix);
	//�õ�����
	virtual osg::Matrixd getMatrix() const;
	//�õ������
	virtual osg::Matrixd getInverseMatrix() const;

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

private:
	osg::ref_ptr<osgViewer::Viewer> m_pHostViewer;
	//�ƶ��ٶ�
	float m_fMoveSpeed;
	osg::Vec3 m_vPosition;
	osg::Vec3 m_vRotation;
public:
	//�������Ƿ���
	bool m_bLeftButtonDown;
	//���X��Y
	float m_fpushY;
	float m_fpushX;
	//��Ļ�Ƕ�
	float m_fAngle;
	//λ�ñ任����
	void ChangePosition(osg::Vec3& delta);
	//��ײ����Ƿ���
	bool m_bPeng;
	//�����ٶ�
	float getSpeed();
	void setSpeed(float&);
	//������ʼλ��
	void setPosition(osg::Vec3 &position);
	osg::Vec3 GetPosition();
};

