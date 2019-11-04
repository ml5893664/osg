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

//����һ��ȫ�ֱ���
osg::ref_ptr<osg::Image> image = new osg::Image;

//�õ�ץͼ
struct CaptureDrawCallback:public osg::Camera::DrawCallback 
{
	CaptureDrawCallback(osg::ref_ptr<osg::Image> image)
	{
		_image = image;
	}
	~CaptureDrawCallback(){}
	virtual void operator()(const osg::Camera& camera)const
	{
		//�õ�����ϵͳ�ӿ�
		osg::ref_ptr<osg::GraphicsContext::WindowingSystemInterface> wsi = osg::GraphicsContext::getWindowingSystemInterface();
		unsigned int width, height;
		//�õ��ֱ���
		wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
		//����һ��image
		_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
		//��ȡ������Ϣץͼ
		_image->readPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	}
	osg::ref_ptr<osg::Image> _image;
};
//ץͼ�¼�������
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
	//����handle��������
	bool ImageHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osg::ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		if (viewer==NULL)
		{
			return false;
		}
		//����һ����̬����
		static int _screenCaptureSequence = 0;
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
		{
												if (ea.getKey()=='c'||ea.getKey()=='C')
												{
													char filename[128];
													//ȷ��һ��������ļ������Ա�֤�ܹ�����ץͼ
													sprintf(filename, "ScreenShot%04d.bmp", _screenCaptureSequence);
													_screenCaptureSequence++;
													//д���ļ�
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
			//��갴��
		case(osgGA::GUIEventAdapter::PUSH):
		{
											  //�������λ��
											  _mx = ea.getX();
											  _my = ea.getY();
											  break;
		}
		case(osgGA::GUIEventAdapter::RELEASE) :
		{
												  if (_mx==ea.getX()&&_my==ea.getY())
												  {
													  //ִ�ж���ѡȡ
													  pick(view, ea.getX(), ea.getY());
												  }
												  break;
		}
		default:
			break;
		}
		return false;
	}
	//����ѡȡ�¼�������
	void pick(osg::ref_ptr<osgViewer::View> view, float x, float y)
	{
		osg::ref_ptr<osg::Node> node = new osg::Node;
		osg::ref_ptr<osg::Group> parent = new osg::Group;
		//����һ���߶ν�����⺯��
		osgUtil::LineSegmentIntersector::Intersections intersections;
		if (view->computeIntersections(x,y,intersections))
		{
			osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
			osg::NodePath& nodePath = intersection.nodePath;
			//�õ�ѡ�������
			node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
			parent = (nodePath.size() >= 2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
		}
		//��һ�ָ�����ʾ����ʾ�����Ѿ���ѡ��
		if (parent&&node)
		{
			osg::ref_ptr<osgFX::Scribe> parentAsScribe = dynamic_cast<osgFX::Scribe*>(parent.get());
			if (!parentAsScribe)
			{
				//�������ѡ������������ʾ
				osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe;
				scribe->addChild(node);
				parent->replaceChild(node, scribe);
			}
			else
			{
				//���û��ѡ���������Ƴ�������ʾ�Ķ���
				osg::Node::ParentList parentList = parentAsScribe->getParents();
				for (osg::Node::ParentList::iterator itr = parentList.begin(); itr != parentList.end();++itr)
				{
					(*itr)->replaceChild(parentAsScribe, node);
				}
			}
		}
	}

public:
	//�õ����λ��
	float _mx;
	float _my;

};


int main()
{
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//����һ���ü�ƽ��
	osg::ref_ptr<osg::ClipPlane> cp1 = new osg::ClipPlane;
	//���òü�ƽ��
	cp1->setClipPlane(0, 1, 1, 1);
	//ָ��ƽ������
	cp1->setClipPlaneNum(0);
	//����һ���ü�ƽ��
	osg::ref_ptr<osg::ClipPlane> cp2 = new osg::ClipPlane;
	//���òü�ƽ��
	cp2->setClipPlane(1, 0, 0, 1);
	//ָ��ƽ������
	cp2->setClipPlaneNum(1);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ȡģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	//���òü�ƽ��
	root->getOrCreateStateSet()->setAttributeAndModes(cp1, osg::StateAttribute::ON);
	root->getOrCreateStateSet()->setAttributeAndModes(cp2, osg::StateAttribute::ON);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�ü�ƽ��1
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//����һ���ü���
	osg::ref_ptr<osg::Scissor> cc = new osg::Scissor;
	//���òü��������Ϣ�����½����ꡢ��Ⱥ͸߶ȣ�
	cc->setScissor(150, 150, 800, 600);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	//���òü�ƽ��
	root->getOrCreateStateSet()->setAttributeAndModes(cc, osg::StateAttribute::ON);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif // �ü�ƽ��2
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	//����ͼ�λ�������
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	traits->width = 1000;
	traits->height = 800;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	//����ͼ�λ�������
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
	if (gc->valid())
	{
		osg::notify(osg::INFO) << "GraphicsWindow has been created successfully." << std::endl;
		//���������ɫ�������ɫ����Ȼ���
		gc->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.6f, 1.0f));
		gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		osg::notify(osg::NOTICE) << "GraphicsWindow has not been created successfully." << std::endl;
	}
	//���ݷֱ���ȷ�����ʵ�ͶӰ����֤��ʾ��ͼ�β�����
	double fovy, aspectRatio, zNear, zFar;
	viewer->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
	double newAspectRatio = double(traits->width) / double(traits->height);
	double aspectRatioChange = newAspectRatio / aspectRatio;
	if (aspectRatioChange != 1.0)
	{
		//����ͶӰ����
		viewer->getCamera()->getProjectionMatrix() *= osg::Matrix::scale(1.0 / aspectRatioChange, 1.0, 1.0);
	}
	//�����ӿ�
	viewer->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	//����ͼ�λ���
	viewer->getCamera()->setGraphicsContext(gc);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //��������
#if 0
	//����CompositeViewer����
	osg::ref_ptr<osgViewer::CompositeViewer> viewer = new osgViewer::CompositeViewer;
	//��ȡţ��ģ��
	osg::ref_ptr<osg::Node> cow = osgDB::readNodeFile("cow.osg");
	//��ȡ�ɻ�ģ��
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
	//����ͼ�λ�������
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 100;
	traits->y = 100;
	traits->width = 900;
	traits->height = 700;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	//����ͼ�λ�������
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
	if (gc->valid())
	{
		osg::notify(osg::INFO) << "GraphicsWindow has been created successfully." << std::endl;
		//���������ɫ�������ɫ����Ȼ���
		gc->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.6f, 1.0f));
		gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		osg::notify(osg::NOTICE) << "GraphicsWindow has not been created successfully." << std::endl;
	}
	//��ͼһ
	{
		//������ͼһ
		osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
		viewer->addView(view);
		//������ͼ��������
		view->setSceneData(cow);
		//��������ӿڼ�ͼ�λ���
		view->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width / 2, traits->height / 2));
		view->getCamera()->setGraphicsContext(gc);
		//���ò�����
		view->setCameraManipulator(new osgGA::TrackballManipulator);
		//����¼�����
		view->addEventHandler(new osgViewer::StatsHandler);
		view->addEventHandler(new osgViewer::WindowSizeHandler);
		view->addEventHandler(new osgViewer::ThreadingHandler);
		view->addEventHandler(new osgViewer::RecordCameraPathHandler);
	}
	//��ͼ��
	{
		osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
		viewer->addView(view);
		view->setSceneData(cessna);
		view->getCamera()->setViewport(new osg::Viewport(traits->width / 2, 0, traits->width / 2, traits->height / 2));
		view->getCamera()->setGraphicsContext(gc);
		view->setCameraManipulator(new osgGA::TrackballManipulator);
	}
	//��ͼ��
	{
		osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
		viewer->addView(view);
		view->setSceneData(cessna);
		//���ݷֱ���ȷ�����ʵ�ͶӰ����֤��ʾ��ͼ�β�����
		double fovy, aspectRatio, zNear, zFar;
		view->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
		double newAspectRatio = double(traits->width) / double(traits->height / 2);
		double aspectRatioChange = newAspectRatio / aspectRatio;
		if (aspectRatioChange != 1.0)
		{
			view->getCamera()->getProjectionMatrix() *= osg::Matrix::scale(1.0 / aspectRatioChange, 1.0, 1.0);
		}
		//�����ӿ�
		view->getCamera()->setViewport(new osg::Viewport(0, traits->height / 2, traits->width, traits->height / 2));
		view->getCamera()->setGraphicsContext(gc);
		view->setCameraManipulator(new osgGA::TrackballManipulator);
	}
	viewer->run();
	return 0;
#endif //����ͼ�����Ⱦ
#if 0
	//����һ��CompositeViewer����
	osg::ref_ptr<osgViewer::CompositeViewer> viewer = new osgViewer::CompositeViewer;
	//����������ͼ
	osg::ref_ptr<osgViewer::View> viewer1 = new osgViewer::View();
	osg::ref_ptr<osgViewer::View> viewer2 = new osgViewer::View();

	int xoffset = 50;
	int yoffset = 200;
	//��ͼһ�Ľڵ�
	osg::ref_ptr<osg::Node> viewer1Node = osgDB::readNodeFile("cessna.osg");
	//��ͼ���Ľڵ�
	osg::ref_ptr<osg::Node> viewer2Node = osgDB::readNodeFile("cow.osg");
	//��ͼ����һ
	{
		//����ͼ�λ�������
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		traits->x = xoffset + 0;
		traits->y = yoffset + 0;
		traits->width = 600;
		traits->height = 480;
		traits->windowDecoration = true;
		traits->doubleBuffer = true;
		traits->sharedContext = 0;
		//����ͼ�λ���
		osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
		//�������
		osg::ref_ptr<osg::Camera> camera1 = viewer1->getCamera();
		//����ͼ�λ���
		camera1->setGraphicsContext(gc);
		//�����ӿ�
		camera1->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
		//���û���
		GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
		camera1->setDrawBuffer(buffer);
		camera1->setReadBuffer(buffer);
		//���ó�������
		viewer1->setSceneData(viewer1Node);
	}
	//��ͼ���ڶ�
	{
		//����ͼ�λ�������
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		traits->x = xoffset + 600;
		traits->y = yoffset + 0;
		traits->width = 600;
		traits->height = 480;
		traits->windowDecoration = true;
		traits->doubleBuffer = true;
		traits->sharedContext = 0;
		//����ͼ�λ���
		osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
		//�������
		osg::ref_ptr<osg::Camera> camera2 = viewer2->getCamera();
		//����ͼ�λ���
		camera2->setGraphicsContext(gc);
		//�����ӿ�
		camera2->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
		//���û���
		GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
		camera2->setDrawBuffer(buffer);
		camera2->setReadBuffer(buffer);
		//���ó�������
		viewer2->setSceneData(viewer2Node);
	}

	viewer->addView(viewer1);
	viewer->addView(viewer2);
	viewer->run();
	return 0;
#endif //����ͼ�ര����Ⱦ
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	//���û��ƻص�
	viewer->getCamera()->setPostDrawCallback(new CaptureDrawCallback(image));
	viewer->setSceneData(root);
	//���ץͼ�¼�
	viewer->addEventHandler(new ImageHandler());
	viewer->run();
	return 0;
#endif //ץͼ
#if 0
	//����Viewer���󣬳��������
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//����������볡����
	TravelManipulator::TravelToScene(viewer);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ȡ����ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�Զ����������������
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//����һ��������
	osg::ref_ptr<osgGA::AnimationPathManipulator> apm = new osgGA::AnimationPathManipulator("animation.path");
	//���ò�����
	viewer->setCameraManipulator(apm);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //·������
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	osg::ref_ptr<osgText::Text> updatetext = new osgText::Text;
	CreateHUD *hudText = new CreateHUD();

	root->addChild(node);
	root->addChild(hudText->createHUD(updatetext));
	//���PICK�¼�������
	viewer->addEventHandler(new PickHandler(updatetext));
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //��ʾλ�ü�ʰȡ
#if 0

#endif //����ѡȡ
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	viewer->addEventHandler(new PickHander1);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;

}