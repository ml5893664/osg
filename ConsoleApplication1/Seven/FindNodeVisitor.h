#pragma once
#include <osg/NodeVisitor>
#include <osg/Node>

#include <osgSim/DOFTransform>
#include <iostream>
#include <vector>
#include <string>
class FindNodeVisitor:public osg::NodeVisitor
{
public:
	//���캯��������Ϊ��Ҫ���ҵĽڵ���
	FindNodeVisitor(const std::string &searchName);
	~FindNodeVisitor();

	virtual void apply(osg::Node& node);

	virtual void apply(osg::Geode& node);

	virtual void apply(osg::Transform& node);


	//����Ҫ���ҵĽڵ���
	void setNameToFind(const std::string& searchName);
	//�õ����ҽڵ������ĵ�һ���ڵ�
	osg::Node* getFirst();
	//����һ���ڵ�����
	typedef std::vector<osg::Node*> nodeListType;
	//�õ����ҽڵ�����
	nodeListType& getNodeList()
	{
		return foundNodeList;
	}
private:
	//�ڵ���
	std::string searchForName;
	//����������ҵĽڵ�
	nodeListType foundNodeList;
	
	
};

