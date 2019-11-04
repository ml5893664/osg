#pragma once
#include <osgViewer/Viewer>

#include <osg/Geode>
#include <osg/Node>
#include <osg/Group>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Camera>
#include <osg/io_utils>
#include <osgText/Text>
#include <sstream>
class PickHandler:public osgGA::GUIEventHandler
{
public:
	PickHandler(osg::ref_ptr<osgText::Text> updateText) :_updateText(updateText)
	{
		//
	}
	~PickHandler()
	{
		//
	}

	//�¼�����
    bool handle(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&);
	
	//pick
	virtual void pick(osg::ref_ptr<osgViewer::Viewer> viewer, const osgGA::GUIEventAdapter& ea);

	//������ʾ�ڴ�
	void setLabel(const std::string& name)
	{
		_updateText->setText(name);
	}
protected:
	//�õ���ǰ��ͼ����
	osg::Vec3 position;
	osg::Vec3 center;
	osg::Vec3 up;
	//����һ�����ֶ���
	osg::ref_ptr<osgText::Text> _updateText;

};

//HUD
class CreateHUD
{
public:
	CreateHUD()
	{
		//
	}
	~CreateHUD()
	{
		//
	}

	//����HUD
	osg::ref_ptr<osg::Node> createHUD(osg::ref_ptr<osgText::Text> updateText)
	{
		//����һ�����
		osg::ref_ptr<osg::Camera> hudCamera = new osg::Camera;
		//���þ���֡����
		hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		//������ͶӰ����
		hudCamera->setProjectionMatrixAsOrtho2D(0, 1280, 0, 1024);
		//������ͼ����
		hudCamera->setViewMatrix(osg::Matrix::identity());
		//������Ⱦ˳��ΪPOST
		hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
		//�����Ȼ���
		hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
		//��������
		std::string timesFont("fonts//cour.tif");
		//����λ��
		osg::Vec3 position(700, 900, 0.0);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
		//�رչ���
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		//�ر���Ȳ���
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		geode->addDrawable(updateText);
		hudCamera->addChild(geode);
		updateText->setCharacterSize(20.0f);
		updateText->setFont(timesFont);
		updateText->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		updateText->setText("");
		updateText->setPosition(position);
		//�������ݱ���ΪDYNAMIC
		updateText->setDataVariance(osg::Object::DYNAMIC);
		return hudCamera;
	}

};
