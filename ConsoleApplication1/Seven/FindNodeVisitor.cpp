#include "FindNodeVisitor.h"


FindNodeVisitor::FindNodeVisitor(const std::string& searchName) :osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), searchForName(searchName)
{
	//
}


FindNodeVisitor::~FindNodeVisitor()
{
}

void FindNodeVisitor::apply(osg::Node& searchNode)
{
	//判断节点名称是否与查找的节点名称一样
	if (searchNode.getName()==searchForName)
	{
		//添加到节点系列
		foundNodeList.push_back(&searchNode);
	}
	//继续遍历
	traverse(searchNode);
}

void FindNodeVisitor::apply(osg::Geode& geode)
{
	apply((osg::Node&)geode);
	traverse((osg::Node&)geode);
}

void FindNodeVisitor::apply(osg::Transform& searchNode)
{
	apply((osg::Node&)searchNode);
	traverse(searchNode);
}
//设置要查找的节点名称
void FindNodeVisitor::setNameToFind(const std::string& searchName)
{
	searchForName = searchName;
	foundNodeList.clear();
}
//得到查找节点向量中的第一个节点
osg::Node* FindNodeVisitor::getFirst()
{
	return *(foundNodeList.begin());
}
