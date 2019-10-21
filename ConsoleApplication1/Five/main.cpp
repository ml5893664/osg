#include <osgViewer/Viewer>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/ClipNode>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>

#include <osgDB//WriteFile>
#include <osgDB//ReadFile>

#include <osgUtil/Optimizer>

#include <iostream>

osg::ref_ptr<osg::Node> createClipNode(osg::ref_ptr<osg::Node> subgraph)
{
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//��������λ���ģʽ������ͷ��涼����
	osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
	polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	//���ö�������λ���ģʽ�����ƶ�״̬�̳�����ΪOVERRIDE
	stateset->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	//��������λ��ƽڵ�
	osg::ref_ptr<osg::Group> wireframe_subgraph = new osg::Group;
	//������Ⱦ״̬
	wireframe_subgraph->setStateSet(stateset);
	wireframe_subgraph->addChild(subgraph);

	root->addChild(wireframe_subgraph);

	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
	//���»ص���ʵ�ֶ�̬�ü�
	osg::ref_ptr<osg::NodeCallback> nc = new osg::AnimationPathCallback(subgraph->getBound().center(), osg::Vec3(0.0f, 0.0f, 1.0f), osg::inDegrees(45.0f));
	transform->setUpdateCallback(nc);

	//�����ü��ڵ�
	osg::ref_ptr<osg::ClipNode> clipnode = new osg::ClipNode;
	osg::BoundingSphere bs = subgraph->getBound();
	bs.radius() *= 0.4f;
	//���òü��ڵ�İ�Χ��
	osg::BoundingBox bb;
	bb.expandBy(bs); 
	//����ǰ��ָ���İ�Χ�д���6���ü�ƽ��
	clipnode->createClipBox(bb);
	//���ü�ѡ
	clipnode->setCullingActive(false);
	transform->addChild(clipnode);
	root->addChild(transform);
	//����δ���ü��Ľڵ�
	osg::ref_ptr<osg::Group> clippedNode = new osg::Group;
	clippedNode->setStateSet(clipnode->getStateSet());
	clippedNode->addChild(subgraph);

	root->addChild(clippedNode);
	return root;
}

int main()
{
#if 0

#endif //��Ⱦ״̬
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Node> root = new osg::Node;
	//����ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");
	root = createClipNode(node);

	viewer->setSceneData(root);
	viewer->run();
	return 0;
}