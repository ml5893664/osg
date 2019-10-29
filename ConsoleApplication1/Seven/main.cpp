#include <osgViewer/Viewer>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/NodeCallback>
#include <osg/MatrixTransform>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>

#include <osgUtil/Optimizer>

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osgGA/EventVisitor>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include "FindNodeVisitor.h"

//顶点访问器，继承自osg::NodeVisitor类
class VertexVisitor :public osg::NodeVisitor
{
public:
	//保存顶点数据
	osg::ref_ptr<osg::Vec3Array> extracted_verts;
	//构造函数，初始化并设置为遍历所有子节点
	VertexVisitor() :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		extracted_verts = new osg::Vec3Array;
	}
	//重载apply（）方法
	void apply(osg::Geode& geode)
	{
		//得到每一个drawable
		for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
		{
			//得到几何体
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
			if (!geom)
			{
				std::cout << "几何体错误！" << std::endl;
				continue;
			}
			//得到顶点数组
			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			if (!verts)
			{
				std::cout << "顶点数组错误！" << std::endl;
				continue;
			}
			//添加到extracted_verts
			extracted_verts->insert(extracted_verts->end(), verts->begin(), verts->end());
		}
	}
};
//节点纹理访问器
class TextureVisitor :public osg::NodeVisitor
{
public:
	//构造函数，遍历所有子节点
	TextureVisitor() :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		//
	}
	//重载apply（）方法
	virtual void apply(osg::Node& node)
	{
		if (node.getStateSet())
		{
			apply(node.getStateSet());
		}
		//实现继续遍历节点
		traverse(node);
	}
	//重载apply（）方法
	virtual void apply(osg::Geode& geode)
	{
		if (geode.getStateSet())
		{
			apply(geode.getStateSet());
		}
		unsigned int cnt = geode.getNumDrawables();
		for (unsigned int i = 0; i < cnt; i++)
		{
			apply(geode.getDrawable(i)->getStateSet());
		}
		traverse(geode);
	}
	//得到贴图列表
	void apply(osg::StateSet* state)
	{
		//得到纹理属性列表
		osg::StateSet::TextureAttributeList& texAttribList = state->getTextureAttributeList();
		for (unsigned int i = 0; i < texAttribList.size(); i++)
		{
			//得到纹理
			osg::Texture2D* tex2D = NULL;
			if (tex2D = dynamic_cast<osg::Texture2D*>(state->getTextureAttribute(i, osg::StateAttribute::TEXTURE)))
			{
				//得到贴图
				if (tex2D->getImage())
				{
					//写入映射表
					_imageList.insert(std::make_pair(tex2D->getImage()->getFileName(), tex2D->getImage()));
				}
			}
		}
	}
	//得到贴图
	std::map<std::string, osg::Image*>& getImages(void)
	{
		return _imageList;
	}
protected:
	//贴图映射表，用来保存贴图名和贴图
	std::map<std::string, osg::Image*> _imageList;
};
//从osg::NodeCallback继承一个新类，实现spaceship的回调
class SpaceShipCallback :public osg::NodeCallback
{
public:
	SpaceShipCallback() :angle(0)
	{
		//
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		//创建矩阵变换节点
		osg::ref_ptr<osg::MatrixTransform> mtCow = dynamic_cast<osg::MatrixTransform*>(node);
		//创建矩阵
		osg::Matrix mr;
		//旋转，绕Z轴，每次旋转angle角度
		mr.makeRotate(angle, osg::Vec3(0.0f, 0.0f, 1.0f));
		//设置矩阵
		mtCow->setMatrix(mr);
		angle += 0.01;
		//继续传递参数，执行其他设置回调的节点
		traverse(node, nv);
	}
public:
	//旋转角度
	double angle;
};
//继承自osg::NodeCallback类，写一个事件回调类
//class MyEventCallback :public osg::NodeCallback
//{
//public:
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		//判断访问器类型
//		if (nv->getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
//		{
//			//创建一个事件访问器并初始化
//			osg::ref_ptr<osgGA::EventVisitor> ev = dynamic_cast<osgGA::EventVisitor*>(nv);
//			if (ev)
//			{
//				//得到执行动作
//				osgGA::GUIActionAdapter* aa = ev->getActionAdapter();
//				//得到事件队列
//				osgGA::EventQueue::Events& events = ev->getEvents();
//				for (osgGA::EventQueue::Events::iterator itr = events.begin(); itr != events.end(); ++itr)
//				{
//					//处理事件
//					handle(*(*itr), *(aa));
//				}
//			}
//		}
//	}
//	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
//	{
//		//得到场景数据
//		osg::ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
//		osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform*>(viewer->getSceneData());
//		switch (ea.getEventType())
//		{
//		case (osgGA::GUIEventAdapter::KEYDOWN) :
//		{
//												   //按下Key_Up键时，向Z正方向平移一个单位
//												   if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
//												   {
//													   osg::Matrix mT;
//													   mT.makeTranslate(0.0f, 0.0f, 1.0f);
//													   mt->setMatrix(mT);
//												   }
//												   //按下Key_Down键时，向Z负方向平移一个单位
//												   if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
//												   {
//													   osg::Matrix mT;
//													   mT.makeTranslate(0.0f, 0.0f, -1.0f);
//													   mt->setMatrix(mT);
//												   }
//												   break;
//		}
//		default:
//			break;
//
//		}
//		return false;
//	}
//};
//从osgDB::Registry::ReadFileCallback,写一个读取文件回调类
class MyReadFileCallback:public osgDB::Registry::ReadFileCallback
{
public:
	//重载readNode()虚函数
	virtual osgDB::ReaderWriter::ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options)
	{
		std::cout << "Starting readNode" << std::endl;
		//创建一个计时器，计算读取模型所耗费的时间
		osg::Timer_t start = osg::Timer::instance()->tick();
		//读取文件名以及操作参数，这个是模型读取的实际函数
		osgDB::ReaderWriter::ReadResult result = osgDB::Registry::instance()->readNodeImplementation(fileName, options);
		std::cout << "Ending readNode" << std::endl;
		//输出读取所耗费的时间
		std::cout << "The time of reading Node" << osg::Timer::instance()->delta_s(start, osg::Timer::instance()->tick()) << " s" << std::endl;
		return result;
	}
};
class MyWriteFileCallback :public osgDB::Registry::WriteFileCallback
{
	virtual osgDB::ReaderWriter::WriteResult writeNode(const osg::Node& obj, const std::string& fileName, const osgDB::ReaderWriter::Options* options)
	{
		std::cout << "Starting writeNode" << std::endl;
		//创建一个计时器，计算写出模型所耗费的时间
		osg::Timer_t start = osg::Timer::instance()->tick();
		//写出文件名以及操作参数，这个是模型写出的实际函数
		osgDB::ReaderWriter::WriteResult result = osgDB::Registry::instance()->writeNodeImplementation(obj,fileName, options);
		std::cout << "Ending writeNode" << std::endl;
		//输出读取所耗费的时间
		std::cout << "The time of writing Node" << osg::Timer::instance()->delta_s(start, osg::Timer::instance()->tick()) << " s" << std::endl;
		return result;
	}

};

int main()
{
#if 0
	//创建Viewer对象，场景浏览器
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("glider.osg");
	root->addChild(node);
	//创建顶点访问器对象
	VertexVisitor vtea;
	//开始执行访问器遍历
	node->accept(vtea);
	//申请一个输出流
	std::ofstream fout("glider.vertexs");
	//得到顶点数组的大小
	int size_t = vtea.extracted_verts->size();
	//初始化一个迭代器
	std::vector<osg::Vec3>::iterator iter = vtea.extracted_verts->begin();
	//写入文件
	for (int i = 0; i < size_t; i++)
	{
		fout << iter->x() << " " << iter->y() << " " << iter->z() << std::endl;
		iter++;
	}
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //顶点访问器
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建一个节点并读取牛的模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//创建纹理访问器
	TextureVisitor textureTV;
	//启动访问器，执行遍历
	node->accept(textureTV);
	//初始化一个贴图映射表
	std::map<std::string, osg::Image*> imageList = textureTV.getImages();
	//初始化一个映射表迭代器
	std::map<std::string, osg::Image*>::iterator iter = imageList.begin();
	unsigned int cnt = 0;
	char* buffer = new char[2000];
	for (; iter != imageList.end(); iter++)
	{
		sprintf(buffer, "TextureImage%d.jpg", cnt++);
		//写入文件
		osgDB::writeImageFile(*iter->second, buffer);
	}
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //纹理访问器
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建一个节点并读取牛的模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//创建节点查找访问器
	FindNodeVisitor cow("cow.osg");
	//启动访问器，开始遍历
	node->accept(cow);
	if (!cow.getFirst())
	{
		std::cout << "无法找到节点，查找失败" << std::endl;
	}
	else
	{
		std::cout << "查找节点成功，成功找到节点" << std::endl;
	}
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //节点访问器
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建一个节点并读取牛的模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	mt->addChild(node);
	mt->setUpdateCallback(new SpaceShipCallback());
	root->addChild(mt);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //节点回调
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建一个节点并读取牛的模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	mt->addChild(node);
	//得到相机
	osg::ref_ptr<osg::Camera> camera = viewer->getCamera();
	//设置相机事件回调
	camera->setEventCallback(new MyEventCallback());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //事件回调
#if 0

#endif //文件读取回调
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//加载读取文件回调类MyReadFileCallback
	osgDB::Registry::instance()->setReadFileCallback(new MyReadFileCallback());
	//加载写出文件回调类MyWriteFileCallback
	osgDB::Registry::instance()->setWriteFileCallback(new MyWriteFileCallback());
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建一个节点并读取牛的模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	osgDB::writeNodeFile(*root, "root.osg");
	viewer->setSceneData(root);
	viewer->run();
	return 0;

}