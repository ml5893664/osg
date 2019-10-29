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
	//构造函数，参数为需要查找的节点名
	FindNodeVisitor(const std::string &searchName);
	~FindNodeVisitor();

	virtual void apply(osg::Node& node);

	virtual void apply(osg::Geode& node);

	virtual void apply(osg::Transform& node);


	//设置要查找的节点名
	void setNameToFind(const std::string& searchName);
	//得到查找节点向量的第一个节点
	osg::Node* getFirst();
	//定义一个节点向量
	typedef std::vector<osg::Node*> nodeListType;
	//得到查找节点向量
	nodeListType& getNodeList()
	{
		return foundNodeList;
	}
private:
	//节点名
	std::string searchForName;
	//用来保存查找的节点
	nodeListType foundNodeList;
	
	
};

