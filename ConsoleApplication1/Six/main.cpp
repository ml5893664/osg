#include <osgViewer/Viewer>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>

#include <osgDB//ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgDB//Registry>

#include <osgUtil/Optimizer>

#include <iostream>
#include <fstream>
#include <memory>
#include <string>

//����һ���ļ�����ģ���࣬�̳����ļ��������ࣨbasic_filebuf��
template<typename Elem,typename Tr=std::char_traits<Elem>>
class progress_streambuf:public std::basic_filebuf<Elem,Tr>
{
public:
	//�����ļ��������ڲ�ʹ��
	typedef std::basic_filebuf<Elem, Tr> base_type;
	//��Ҫ��ʽת��
	explicit progress_streambuf(const std::string &filename)
		:base_type(),
		count_(0),
		prev_pere_(0)
	{
		//���ļ�
		if (open(filename.c_str(),std::ios_base::in|std::ios_base::binary))
		{
			//�õ��ļ���С
			size_ = static_cast<int>(pubseekoff(0, std::ios_base::end, std::ios_base::in));
			//�ļ�ָ���Ƶ���λ
			pubseekoff(0, std::ios_base::beg, std::ios_base::in);
		}
	}
protected:
   //���ļ������ݶ��뻺��
	virtual int_type uflow()
	{
		//���뻺��
		int_type v = base_type::uflow();
		//��ǰ��ȡ�Ĵ�С
		//egptr()���ڷ��ص�ǰ�������ָ��
		//gptr()���������嵱ǰָ��
		count_ += egptr() - gptr();
		int p = count_ * 100 / size_;
		if (p>prev_pere_)
		{
			std::cout << p << " ";
			prev_pere_ = p;
		}
		return v;
	}

private:
	//����
	int count_;
	//�ļ��Ĵ�С
	int size_;
	//��ǰ����
	int prev_pere_;

};

typedef progress_streambuf<char> progbuf;

int main()
{
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(node);

	osgDB::writeNodeFile(*root, "root.osg");
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�ļ���ȡ
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	std::string filename = "cow.osg";
	//�����ܷ��ҵ���ȡ���ļ��Ĳ��
	osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension(osgDB::getLowerCaseFileExtension(filename));
	if (!rw.get())
	{
		//�Ҳ�����Ӧ�Ĳ����ȡ�ļ�
		std::cerr << "Error:could not find a suitable reader/writer to load the specified file" << std::endl;
		return 1;
	}
	//����һ���ļ���ȡ���ȶ���
	std::auto_ptr<progbuf> pb(new progbuf(osgDB::findDataFile(filename)));
	if (pb->is_open())
	{
		//���ļ�ʧ��
		std::cerr << "Error:could not open file" << filename << "" << std::endl;
		return 1;
	}
	std::cout << "��ȡ���ȣ�" << std::endl;
	//��ʼ��һ���ļ�������
	std::istream mis(pb.get());
	//��ȡ�ڵ�
	osgDB::ReaderWriter::ReadResult rr = rw->readNode(mis);
	//�õ��ڵ�
	osg::ref_ptr<osg::Node> node = rr.getNode();
	if (!node)
	{
		//��ȡʧ��
		std::cerr << "Error:could not open file" << filename << "" << std::endl;
	}
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�ļ���ȡ����
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osgDB::Registry::instance()->addFileExtensionAlias("VR", "VR");
	//����������ڵ�
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.VR");
	root->addChild(node);
	//���浱ǰ����
	osgDB::writeNodeFile(*node, "scene.VR");
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�Զ����ļ���ʽ��д���
#if 0

#endif //��д�����ļ���������·������
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node1 = new osg::Node;
	/*
	�ܹؼ���һ������������ʶ��unicode�����ַ�
	*/
	setlocale(LC_ALL, "chs");
	node1 = osgDB::readNodeFile("ţ.ive");
	osg::ref_ptr<osg::Node> node2 = osgDB::readNodeFile("cessna.osg");

	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	pat->addChild(node2);
	pat->setPosition(osg::Vec3(10.0f, 0.0f, 0.0f));
	root->addChild(node1);
	root->addChild(pat);
	osgDB::writeNodeFile(*root, "ţ�ͷɻ�.ive");
	viewer->setSceneData(root);
	viewer->run();
	return 0;



}