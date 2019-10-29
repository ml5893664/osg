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

//������������̳���osg::NodeVisitor��
class VertexVisitor :public osg::NodeVisitor
{
public:
	//���涥������
	osg::ref_ptr<osg::Vec3Array> extracted_verts;
	//���캯������ʼ��������Ϊ���������ӽڵ�
	VertexVisitor() :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		extracted_verts = new osg::Vec3Array;
	}
	//����apply��������
	void apply(osg::Geode& geode)
	{
		//�õ�ÿһ��drawable
		for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
		{
			//�õ�������
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
			if (!geom)
			{
				std::cout << "���������" << std::endl;
				continue;
			}
			//�õ���������
			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			if (!verts)
			{
				std::cout << "�����������" << std::endl;
				continue;
			}
			//��ӵ�extracted_verts
			extracted_verts->insert(extracted_verts->end(), verts->begin(), verts->end());
		}
	}
};
//�ڵ����������
class TextureVisitor :public osg::NodeVisitor
{
public:
	//���캯�������������ӽڵ�
	TextureVisitor() :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		//
	}
	//����apply��������
	virtual void apply(osg::Node& node)
	{
		if (node.getStateSet())
		{
			apply(node.getStateSet());
		}
		//ʵ�ּ��������ڵ�
		traverse(node);
	}
	//����apply��������
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
	//�õ���ͼ�б�
	void apply(osg::StateSet* state)
	{
		//�õ����������б�
		osg::StateSet::TextureAttributeList& texAttribList = state->getTextureAttributeList();
		for (unsigned int i = 0; i < texAttribList.size(); i++)
		{
			//�õ�����
			osg::Texture2D* tex2D = NULL;
			if (tex2D = dynamic_cast<osg::Texture2D*>(state->getTextureAttribute(i, osg::StateAttribute::TEXTURE)))
			{
				//�õ���ͼ
				if (tex2D->getImage())
				{
					//д��ӳ���
					_imageList.insert(std::make_pair(tex2D->getImage()->getFileName(), tex2D->getImage()));
				}
			}
		}
	}
	//�õ���ͼ
	std::map<std::string, osg::Image*>& getImages(void)
	{
		return _imageList;
	}
protected:
	//��ͼӳ�������������ͼ������ͼ
	std::map<std::string, osg::Image*> _imageList;
};
//��osg::NodeCallback�̳�һ�����࣬ʵ��spaceship�Ļص�
class SpaceShipCallback :public osg::NodeCallback
{
public:
	SpaceShipCallback() :angle(0)
	{
		//
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		//��������任�ڵ�
		osg::ref_ptr<osg::MatrixTransform> mtCow = dynamic_cast<osg::MatrixTransform*>(node);
		//��������
		osg::Matrix mr;
		//��ת����Z�ᣬÿ����תangle�Ƕ�
		mr.makeRotate(angle, osg::Vec3(0.0f, 0.0f, 1.0f));
		//���þ���
		mtCow->setMatrix(mr);
		angle += 0.01;
		//�������ݲ�����ִ���������ûص��Ľڵ�
		traverse(node, nv);
	}
public:
	//��ת�Ƕ�
	double angle;
};
//�̳���osg::NodeCallback�࣬дһ���¼��ص���
//class MyEventCallback :public osg::NodeCallback
//{
//public:
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		//�жϷ���������
//		if (nv->getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
//		{
//			//����һ���¼�����������ʼ��
//			osg::ref_ptr<osgGA::EventVisitor> ev = dynamic_cast<osgGA::EventVisitor*>(nv);
//			if (ev)
//			{
//				//�õ�ִ�ж���
//				osgGA::GUIActionAdapter* aa = ev->getActionAdapter();
//				//�õ��¼�����
//				osgGA::EventQueue::Events& events = ev->getEvents();
//				for (osgGA::EventQueue::Events::iterator itr = events.begin(); itr != events.end(); ++itr)
//				{
//					//�����¼�
//					handle(*(*itr), *(aa));
//				}
//			}
//		}
//	}
//	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
//	{
//		//�õ���������
//		osg::ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
//		osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform*>(viewer->getSceneData());
//		switch (ea.getEventType())
//		{
//		case (osgGA::GUIEventAdapter::KEYDOWN) :
//		{
//												   //����Key_Up��ʱ����Z������ƽ��һ����λ
//												   if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
//												   {
//													   osg::Matrix mT;
//													   mT.makeTranslate(0.0f, 0.0f, 1.0f);
//													   mt->setMatrix(mT);
//												   }
//												   //����Key_Down��ʱ����Z������ƽ��һ����λ
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
//��osgDB::Registry::ReadFileCallback,дһ����ȡ�ļ��ص���
class MyReadFileCallback:public osgDB::Registry::ReadFileCallback
{
public:
	//����readNode()�麯��
	virtual osgDB::ReaderWriter::ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options)
	{
		std::cout << "Starting readNode" << std::endl;
		//����һ����ʱ���������ȡģ�����ķѵ�ʱ��
		osg::Timer_t start = osg::Timer::instance()->tick();
		//��ȡ�ļ����Լ����������������ģ�Ͷ�ȡ��ʵ�ʺ���
		osgDB::ReaderWriter::ReadResult result = osgDB::Registry::instance()->readNodeImplementation(fileName, options);
		std::cout << "Ending readNode" << std::endl;
		//�����ȡ���ķѵ�ʱ��
		std::cout << "The time of reading Node" << osg::Timer::instance()->delta_s(start, osg::Timer::instance()->tick()) << " s" << std::endl;
		return result;
	}
};
class MyWriteFileCallback :public osgDB::Registry::WriteFileCallback
{
	virtual osgDB::ReaderWriter::WriteResult writeNode(const osg::Node& obj, const std::string& fileName, const osgDB::ReaderWriter::Options* options)
	{
		std::cout << "Starting writeNode" << std::endl;
		//����һ����ʱ��������д��ģ�����ķѵ�ʱ��
		osg::Timer_t start = osg::Timer::instance()->tick();
		//д���ļ����Լ����������������ģ��д����ʵ�ʺ���
		osgDB::ReaderWriter::WriteResult result = osgDB::Registry::instance()->writeNodeImplementation(obj,fileName, options);
		std::cout << "Ending writeNode" << std::endl;
		//�����ȡ���ķѵ�ʱ��
		std::cout << "The time of writing Node" << osg::Timer::instance()->delta_s(start, osg::Timer::instance()->tick()) << " s" << std::endl;
		return result;
	}

};

int main()
{
#if 0
	//����Viewer���󣬳��������
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("glider.osg");
	root->addChild(node);
	//�����������������
	VertexVisitor vtea;
	//��ʼִ�з���������
	node->accept(vtea);
	//����һ�������
	std::ofstream fout("glider.vertexs");
	//�õ���������Ĵ�С
	int size_t = vtea.extracted_verts->size();
	//��ʼ��һ��������
	std::vector<osg::Vec3>::iterator iter = vtea.extracted_verts->begin();
	//д���ļ�
	for (int i = 0; i < size_t; i++)
	{
		fout << iter->x() << " " << iter->y() << " " << iter->z() << std::endl;
		iter++;
	}
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //���������
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ���ڵ㲢��ȡţ��ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//�������������
	TextureVisitor textureTV;
	//������������ִ�б���
	node->accept(textureTV);
	//��ʼ��һ����ͼӳ���
	std::map<std::string, osg::Image*> imageList = textureTV.getImages();
	//��ʼ��һ��ӳ��������
	std::map<std::string, osg::Image*>::iterator iter = imageList.begin();
	unsigned int cnt = 0;
	char* buffer = new char[2000];
	for (; iter != imageList.end(); iter++)
	{
		sprintf(buffer, "TextureImage%d.jpg", cnt++);
		//д���ļ�
		osgDB::writeImageFile(*iter->second, buffer);
	}
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //���������
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ���ڵ㲢��ȡţ��ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//�����ڵ���ҷ�����
	FindNodeVisitor cow("cow.osg");
	//��������������ʼ����
	node->accept(cow);
	if (!cow.getFirst())
	{
		std::cout << "�޷��ҵ��ڵ㣬����ʧ��" << std::endl;
	}
	else
	{
		std::cout << "���ҽڵ�ɹ����ɹ��ҵ��ڵ�" << std::endl;
	}
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�ڵ������
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ���ڵ㲢��ȡţ��ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	mt->addChild(node);
	mt->setUpdateCallback(new SpaceShipCallback());
	root->addChild(mt);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�ڵ�ص�
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ���ڵ㲢��ȡţ��ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	mt->addChild(node);
	//�õ����
	osg::ref_ptr<osg::Camera> camera = viewer->getCamera();
	//��������¼��ص�
	camera->setEventCallback(new MyEventCallback());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�¼��ص�
#if 0

#endif //�ļ���ȡ�ص�
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//���ض�ȡ�ļ��ص���MyReadFileCallback
	osgDB::Registry::instance()->setReadFileCallback(new MyReadFileCallback());
	//����д���ļ��ص���MyWriteFileCallback
	osgDB::Registry::instance()->setWriteFileCallback(new MyWriteFileCallback());
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ���ڵ㲢��ȡţ��ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);
	osgDB::writeNodeFile(*root, "root.osg");
	viewer->setSceneData(root);
	viewer->run();
	return 0;

}