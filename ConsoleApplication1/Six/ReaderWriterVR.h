#pragma once
#include <osg/Notify>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>

#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <iostream>
#include <fstream>
#include <istream>

#define N 10

class ReaderWriterVR :public osgDB::ReaderWriter
{
public:
	ReaderWriterVR();
	~ReaderWriterVR();

	//插件类名称
	const char* className() const{ return "VR Reader/Writer"; }
	//检查插件是否支持该扩展名
	bool acceptsExtension(const std::string& extension) const
	{
		return osgDB::equalCaseInsensitive(extension, "VR");
	}

	//读取节点
	ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
	{
		//得到文件名
		std::string ext(osgDB::getLowerCaseFileExtension(fileName));
		//判断是否支持扩展名
		if (!acceptsExtension(ext))
		{
			return ReadResult::FILE_NOT_HANDLED;
		}
		osg::notify(osg::INFO) << "ReaderWriterVR(\"" << fileName << "\")" << std::endl;
		osg::ref_ptr<osg::Node> node = new osg::Node;
		std::string tempFileName("CopyRight.ive");
		//申请一个输入流
		std::ifstream in(fileName.c_str(), std::ios::in | std::ios::binary);
		if (!in)
		{
			osg::notify(osg::WARN) << "Subfile could not be loaded" << std::endl;
		}
		//申请一个输出流，用中转保存
		//下面主要是C++文件流的操作，不作过多的解释了
		std::ofstream fout(tempFileName.c_str(), std::ios::out | std::ios::binary);
		//if (in==NULL)
		//{
		//	std::cout << "请确定所读的文件已存在或者文件名是否正确" << std::endl;
		//	exit(-1);
		//}
		int j = 0;
		char fileContent;
		while (!in.eof())
		{
			j++;
			if (j!=N)
			{
				in.get(fileContent);
				fout.put(fileContent);
			}
			else
			{
				in.get(fileContent);
			}
			fileContent = ' ';
		}
		in.close();
		fout.close();
		node = osgDB::readNodeFile(tempFileName);
		std::ofstream foutCopyRight(tempFileName.c_str(), std::ios::out | std::ios::binary);
		foutCopyRight << "CopyRight@FlySky" << std::endl;
		foutCopyRight << "Email::zzuxp@163.com" << std::endl;
		foutCopyRight.close();
		return node.get();
	}
	//写出该节点
	WriteResult writeNode(const osg::Node& node, const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
	{
		//得到文件扩展名
		std::string ext = osgDB::getFileExtension(fileName);
		//判断该插件是否支持该扩展名
		if (!acceptsExtension(ext))
		{
			return WriteResult::FILE_NOT_HANDLED;
		}
		std::string dealFileName("CopyRight.ive");
		osgDB::writeNodeFile(node, dealFileName);
		std::ifstream in(dealFileName.c_str(), std::ios::in | std::ios::binary);
		std::ofstream fout(fileName.c_str(), std::ios::out | std::ios::binary);
		//if (in==NULL)
		//{
		//	std::cout << "请确定文件是否存在或者文件名是否正确" << std::endl;
		//	std::cout << "No Data Load" << std::endl;
		//	exit(-1);
		//}
		int i = 0;
		char fileContent;
		while (!in.eof())
		{
			i++;
			if (i==N)
			{
				fout.put('a');
			}
			in.get(fileContent);
			fout.put(fileContent);
			fileContent = ' ';
		}
		in.close();
		fout.close();
		std::ofstream foutCopyRight(dealFileName.c_str(), std::ios::out | std::ios::binary);
		foutCopyRight << "CopyRight@FlySky" << std::endl;
		foutCopyRight << "Email::zzuxp@163.com" << std::endl;
		foutCopyRight.close();
		return WriteResult::FILE_SAVED;
	}
};
//为了实现插件注册，定义全局变量
REGISTER_OSGPLUGIN(VR, ReaderWriterVR)

