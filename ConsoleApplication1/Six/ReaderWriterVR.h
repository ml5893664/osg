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

	//���������
	const char* className() const{ return "VR Reader/Writer"; }
	//������Ƿ�֧�ָ���չ��
	bool acceptsExtension(const std::string& extension) const
	{
		return osgDB::equalCaseInsensitive(extension, "VR");
	}

	//��ȡ�ڵ�
	ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
	{
		//�õ��ļ���
		std::string ext(osgDB::getLowerCaseFileExtension(fileName));
		//�ж��Ƿ�֧����չ��
		if (!acceptsExtension(ext))
		{
			return ReadResult::FILE_NOT_HANDLED;
		}
		osg::notify(osg::INFO) << "ReaderWriterVR(\"" << fileName << "\")" << std::endl;
		osg::ref_ptr<osg::Node> node = new osg::Node;
		std::string tempFileName("CopyRight.ive");
		//����һ��������
		std::ifstream in(fileName.c_str(), std::ios::in | std::ios::binary);
		if (!in)
		{
			osg::notify(osg::WARN) << "Subfile could not be loaded" << std::endl;
		}
		//����һ�������������ת����
		//������Ҫ��C++�ļ����Ĳ�������������Ľ�����
		std::ofstream fout(tempFileName.c_str(), std::ios::out | std::ios::binary);
		//if (in==NULL)
		//{
		//	std::cout << "��ȷ���������ļ��Ѵ��ڻ����ļ����Ƿ���ȷ" << std::endl;
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
	//д���ýڵ�
	WriteResult writeNode(const osg::Node& node, const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
	{
		//�õ��ļ���չ��
		std::string ext = osgDB::getFileExtension(fileName);
		//�жϸò���Ƿ�֧�ָ���չ��
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
		//	std::cout << "��ȷ���ļ��Ƿ���ڻ����ļ����Ƿ���ȷ" << std::endl;
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
//Ϊ��ʵ�ֲ��ע�ᣬ����ȫ�ֱ���
REGISTER_OSGPLUGIN(VR, ReaderWriterVR)

