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

	//事件处理
    bool handle(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&);
	
	//pick
	virtual void pick(osg::ref_ptr<osgViewer::Viewer> viewer, const osgGA::GUIEventAdapter& ea);

	//设置显示内存
	void setLabel(const std::string& name)
	{
		_updateText->setText(name);
	}
protected:
	//得到当前视图矩阵
	osg::Vec3 position;
	osg::Vec3 center;
	osg::Vec3 up;
	//传递一个文字对象
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

	//创建HUD
	osg::ref_ptr<osg::Node> createHUD(osg::ref_ptr<osgText::Text> updateText)
	{
		//创建一个相机
		osg::ref_ptr<osg::Camera> hudCamera = new osg::Camera;
		//设置绝对帧引用
		hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		//设置正投影矩阵
		hudCamera->setProjectionMatrixAsOrtho2D(0, 1280, 0, 1024);
		//设置视图矩阵
		hudCamera->setViewMatrix(osg::Matrix::identity());
		//设置渲染顺序为POST
		hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
		//清除深度缓存
		hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
		//设置字体
		std::string timesFont("fonts//cour.tif");
		//设置位置
		osg::Vec3 position(700, 900, 0.0);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
		//关闭光照
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		//关闭深度测试
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		geode->addDrawable(updateText);
		hudCamera->addChild(geode);
		updateText->setCharacterSize(20.0f);
		updateText->setFont(timesFont);
		updateText->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		updateText->setText("");
		updateText->setPosition(position);
		//设置数据变量为DYNAMIC
		updateText->setDataVariance(osg::Object::DYNAMIC);
		return hudCamera;
	}

};
