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
	//�жϽڵ������Ƿ�����ҵĽڵ�����һ��
	if (searchNode.getName()==searchForName)
	{
		//��ӵ��ڵ�ϵ��
		foundNodeList.push_back(&searchNode);
	}
	//��������
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
//����Ҫ���ҵĽڵ�����
void FindNodeVisitor::setNameToFind(const std::string& searchName)
{
	searchForName = searchName;
	foundNodeList.clear();
}
//�õ����ҽڵ������еĵ�һ���ڵ�
osg::Node* FindNodeVisitor::getFirst()
{
	return *(foundNodeList.begin());
}
