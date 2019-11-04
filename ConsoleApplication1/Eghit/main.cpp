#include <osgViewer//Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/CompositeViewer>
#include <osgViewer/GraphicsWindow>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Node>
#include <osg/ClipPlane>
#include <osg/StateAttribute>
#include <osg/Scissor>
#include <osg/Camera>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/BufferObject>
#include <osg/Image>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/TrackballManipulator>
#include <osgGA/AnimationPathManipulator>

#include <osgUtil/Optimizer>

#include <osgFX/Scribe>

#include <iostream>

#include "TravelManipulator.h"

#include "PickHandler.h"

//定义一个全局变量
osg::ref_ptr<osg::Image> image = new osg::Image;

//得到抓图
struct CaptureDrawCallback:public osg::Camera::DrawCallback 
{
	CaptureDrawCallback(osg::ref_ptr<osg::Image> image)
	{
		_image = image;
	}
	~CaptureDrawCallback(){}
	virtual void operator()(const osg::Camera& camera)const
	{
		//得到窗口系统接口
		osg::ref_ptr<osg::GraphicsContext::WindowingSystemInterface> wsi = osg::GraphicsContext::getWindowingSystemInterface();
		unsigned int width, height;
		//得到分辨率
		wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
		//分配一个image
		_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
		//读取像素信息抓图
		_image->readPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	}
	osg::ref_ptr<osg::Image> _image;
};
//抓图事件处理器
class ImageHandler :public osgGA::GUIEventHandler
{
public:
	ImageHandler()
	{
		//
	}
	~ImageHandler()
	{
		//
	}
	//重载handle（）函数
	bool ImageHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osg::ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		if (viewer==NULL)
		{
			return false;
		}
		//定义一个静态变量
		static int _screenCaptureSequence = 0;
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
		{
												if (ea.getKey()=='c'||ea.getKey()=='C')
												{
													char filename[128];
													//确定一个合理的文件名，以保证能够连续抓图
													sprintf(filename, "ScreenShot%04d.bmp", _screenCaptureSequence);
													_screenCaptureSequence++;
													//写入文件
													osgDB::writeImageFile(*image, filename);
												}
		}
			break;
		default:
			return false;
		}
		return true;

	}
};

class PickHander1 :public osgGA::GUIEventHandler
{
public:
	PickHander1() :_mx(0.0f), _my(0.0f)
	{
		//
	}
	~PickHander1()
	{
		//
	}

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osg::ref_ptr<osgViewer::View> view = dynamic_cast<osgViewer::View*>(&aa);
		if (!view) return false;
		switch (ea.getEventType())
		{
			//鼠标按下
		case(osgGA::GUIEventAdapter::PUSH):
		{
											  //更新鼠标位置
											  _mx = ea.getX();
											  _my = ea.getY();
											  break;
		}
		case(osgGA::GUIEventAdapter::RELEASE) :
		{
												  if (_mx==ea.getX()&&_my==ea.getY())
												  {
													  //执行对象选取
													  pick(view, ea.getX(), ea.getY());
												  }
												  break;
		}
		default:
			break;
		}
		return false;
	}
	//对象选取事件处理器
	void pick(osg::ref_ptr<osgViewer::View> view, float x, float y)
	{
		osg::ref_ptr<osg::Node> node = new osg::Node;
		osg::ref_ptr<osg::Group> parent = new osg::Group;
		//创建一个线段交集检测函数
		osgUtil::LineSegmentIntersector::Intersections intersections;
		if (view->computeIntersections(x,y,intersections))
		{
			osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
			osg::NodePath& nodePath = intersection.nodePath;
			//得到选择的物体
			node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
			parent = (nodePath.size() >= 2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
		}
		//用一种高亮显示来显示物体已经被选中
		if (parent&&node)
		{
			osg::ref_ptr<osgFX::Scribe> parentAsScribe = dynamic_cast<osgFX::Scribe*>(parent.get());
			if (!parentAsScribe)
			{
				//如果对象选择例，高亮显示
				osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe;
				scribe->addChild(node);
				parent->replaceChild(node, scribe);
			}
			else
			{
				//如果没有选择例，则移除高亮显示的对象
				osg::Node::ParentList parentList = parentAsScribe->getParents();
				for (osg::Node::ParentList::iterator itr = parentList.begin(); itr != parentList.end();++itr)
				{
					(*itr)->replaceChild(parentAsScribe, node);
				}
			}
		}
	}

public:
	//得到鼠标位置
	float _mx;
	float _my;

};


int main()
{
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//创建一个裁剪平面
	osg::ref_ptr<osg::ClipPlane> cp1 = new osg::ClipPlane;
	//设置裁剪平面
	cp1->setClipPlane(0, 1, 1, 1);
	//指定平面索引
	cp1->setClipPlaneNum(0);
	//创建一个裁剪平面
	osg::ref_ptr<osg::ClipPlane> cp2 = new osg::ClipPlane;
	//设置裁剪平面
	cp2->setClipPlane(1, 0, 0, 1);
	//指定平面索引
	cp2->setClipPlaneNum(1);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读取模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	//启用裁剪平面
	root->getOrCreateStateSet()->setAttributeAndModes(cp1, osg::StateAttribute::ON);
	root->getOrCreateStateSet()->setAttributeAndModes(cp2, osg::StateAttribute::ON);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //裁剪平面1
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//创建一个裁剪面
	osg::ref_ptr<osg::Scissor> cc = new osg::Scissor;
	//设置裁剪面矩形信息（左下角坐标、宽度和高度）
	cc->setScissor(150, 150, 800, 600);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	//启用裁剪平面
	root->getOrCreateStateSet()->setAttributeAndModes(cc, osg::StateAttribute::ON);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif // 裁剪平面2
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	//设置图形环境特性
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	traits->width = 1000;
	traits->height = 800;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	//创建图形环境特性
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
	if (gc->valid())
	{
		osg::notify(osg::INFO) << "GraphicsWindow has been created successfully." << std::endl;
		//清除窗口颜色及清除颜色和深度缓冲
		gc->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.6f, 1.0f));
		gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		osg::notify(osg::NOTICE) << "GraphicsWindow has not been created successfully." << std::endl;
	}
	//根据分辨率确定合适的投影来保证显示的图形不变形
	double fovy, aspectRatio, zNear, zFar;
	viewer->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
	double newAspectRatio = double(traits->width) / double(traits->height);
	double aspectRatioChange = newAspectRatio / aspectRatio;
	if (aspectRatioChange != 1.0)
	{
		//设置投影矩阵
		viewer->getCamera()->getProjectionMatrix() *= osg::Matrix::scale(1.0 / aspectRatioChange, 1.0, 1.0);
	}
	//设置视口
	viewer->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	//设置图形环境
	viewer->getCamera()->setGraphicsContext(gc);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //宽屏变形
#if 0
	//创建CompositeViewer对象
	osg::ref_ptr<osgViewer::CompositeViewer> viewer = new osgViewer::CompositeViewer;
	//读取牛的模型
	osg::ref_ptr<osg::Node> cow = osgDB::readNodeFile("cow.osg");
	//读取飞机模型
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
	//设置图形环境特性
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 100;
	traits->y = 100;
	traits->width = 900;
	traits->height = 700;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	//创建图形环境特性
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
	if (gc->valid())
	{
		osg::notify(osg::INFO) << "GraphicsWindow has been created successfully." << std::endl;
		//清除窗口颜色及清除颜色和深度缓冲
		gc->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.6f, 1.0f));
		gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		osg::notify(osg::NOTICE) << "GraphicsWindow has not been created successfully." << std::endl;
	}
	//视图一
	{
		//创建视图一
		osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
		viewer->addView(view);
		//设置视图场景数据
		view->setSceneData(cow);
		//设置相机视口及图形环境
		view->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width / 2, traits->height / 2));
		view->getCamera()->setGraphicsContext(gc);
		//设置操作器
		view->setCameraManipulator(new osgGA::TrackballManipulator);
		//添加事件处理
		view->addEventHandler(new osgViewer::StatsHandler);
		view->addEventHandler(new osgViewer::WindowSizeHandler);
		view->addEventHandler(new osgViewer::ThreadingHandler);
		view->addEventHandler(new osgViewer::RecordCameraPathHandler);
	}
	//视图二
	{
		osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
		viewer->addView(view);
		view->setSceneData(cessna);
		view->getCamera()->setViewport(new osg::Viewport(traits->width / 2, 0, traits->width / 2, traits->height / 2));
		view->getCamera()->setGraphicsContext(gc);
		view->setCameraManipulator(new osgGA::TrackballManipulator);
	}
	//视图三
	{
		osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
		viewer->addView(view);
		view->setSceneData(cessna);
		//根据分辨率确定合适的投影来保证显示的图形不变形
		double fovy, aspectRatio, zNear, zFar;
		view->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
		double newAspectRatio = double(traits->width) / double(traits->height / 2);
		double aspectRatioChange = newAspectRatio / aspectRatio;
		if (aspectRatioChange != 1.0)
		{
			view->getCamera()->getProjectionMatrix() *= osg::Matrix::scale(1.0 / aspectRatioChange, 1.0, 1.0);
		}
		//设置视口
		view->getCamera()->setViewport(new osg::Viewport(0, traits->height / 2, traits->width, traits->height / 2));
		view->getCamera()->setGraphicsContext(gc);
		view->setCameraManipulator(new osgGA::TrackballManipulator);
	}
	viewer->run();
	return 0;
#endif //多视图相机渲染
#if 0
	//创建一个CompositeViewer对象
	osg::ref_ptr<osgViewer::CompositeViewer> viewer = new osgViewer::CompositeViewer;
	//创建两个视图
	osg::ref_ptr<osgViewer::View> viewer1 = new osgViewer::View();
	osg::ref_ptr<osgViewer::View> viewer2 = new osgViewer::View();

	int xoffset = 50;
	int yoffset = 200;
	//视图一的节点
	osg::ref_ptr<osg::Node> viewer1Node = osgDB::readNodeFile("cessna.osg");
	//视图二的节点
	osg::ref_ptr<osg::Node> viewer2Node = osgDB::readNodeFile("cow.osg");
	//视图窗口一
	{
		//设置图形环境特性
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		traits->x = xoffset + 0;
		traits->y = yoffset + 0;
		traits->width = 600;
		traits->height = 480;
		traits->windowDecoration = true;
		traits->doubleBuffer = true;
		traits->sharedContext = 0;
		//创建图形环境
		osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
		//创建相机
		osg::ref_ptr<osg::Camera> camera1 = viewer1->getCamera();
		//设置图形环境
		camera1->setGraphicsContext(gc);
		//设置视口
		camera1->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
		//设置缓冲
		GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
		camera1->setDrawBuffer(buffer);
		camera1->setReadBuffer(buffer);
		//设置场景数据
		viewer1->setSceneData(viewer1Node);
	}
	//视图窗口二
	{
		//设置图形环境特性
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		traits->x = xoffset + 600;
		traits->y = yoffset + 0;
		traits->width = 600;
		traits->height = 480;
		traits->windowDecoration = true;
		traits->doubleBuffer = true;
		traits->sharedContext = 0;
		//创建图形环境
		osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
		//创建相机
		osg::ref_ptr<osg::Camera> camera2 = viewer2->getCamera();
		//设置图形环境
		camera2->setGraphicsContext(gc);
		//设置视口
		camera2->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
		//设置缓冲
		GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
		camera2->setDrawBuffer(buffer);
		camera2->setReadBuffer(buffer);
		//设置场景数据
		viewer2->setSceneData(viewer2Node);
	}

	viewer->addView(viewer1);
	viewer->addView(viewer2);
	viewer->run();
	return 0;
#endif //多视图多窗口渲染
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	//设置绘制回调
	viewer->getCamera()->setPostDrawCallback(new CaptureDrawCallback(image));
	viewer->setSceneData(root);
	//添加抓图事件
	viewer->addEventHandler(new ImageHandler());
	viewer->run();
	return 0;
#endif //抓图
#if 0
	//创建Viewer对象，场景浏览器
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//把漫游其加入场景中
	TravelManipulator::TravelToScene(viewer);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读取地形模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //自定义操作器场景漫游
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//申请一个操作器
	osg::ref_ptr<osgGA::AnimationPathManipulator> apm = new osgGA::AnimationPathManipulator("animation.path");
	//启用操作器
	viewer->setCameraManipulator(apm);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //路径漫游
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	osg::ref_ptr<osgText::Text> updatetext = new osgText::Text;
	CreateHUD *hudText = new CreateHUD();

	root->addChild(node);
	root->addChild(hudText->createHUD(updatetext));
	//添加PICK事件处理器
	viewer->addEventHandler(new PickHandler(updatetext));
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //显示位置及拾取
#if 0

#endif //对象选取
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	viewer->addEventHandler(new PickHander1);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;

}