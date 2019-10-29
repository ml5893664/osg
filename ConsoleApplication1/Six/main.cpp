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

//定义一个文件进度模板类，继承自文件流缓冲类（basic_filebuf）
template<typename Elem,typename Tr=std::char_traits<Elem>>
class progress_streambuf:public std::basic_filebuf<Elem,Tr>
{
public:
	//方便文件流缓冲内部使用
	typedef std::basic_filebuf<Elem, Tr> base_type;
	//不要隐式转换
	explicit progress_streambuf(const std::string &filename)
		:base_type(),
		count_(0),
		prev_pere_(0)
	{
		//打开文件
		if (open(filename.c_str(),std::ios_base::in|std::ios_base::binary))
		{
			//得到文件大小
			size_ = static_cast<int>(pubseekoff(0, std::ios_base::end, std::ios_base::in));
			//文件指针移到首位
			pubseekoff(0, std::ios_base::beg, std::ios_base::in);
		}
	}
protected:
   //将文件的内容读入缓冲
	virtual int_type uflow()
	{
		//读入缓冲
		int_type v = base_type::uflow();
		//当前读取的大小
		//egptr()用于返回当前缓冲结束指针
		//gptr()用于流缓冲当前指针
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
	//计数
	int count_;
	//文件的大小
	int size_;
	//当前进度
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
#endif //文件读取
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	std::string filename = "cow.osg";
	//搜索能否找到读取该文件的插件
	osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension(osgDB::getLowerCaseFileExtension(filename));
	if (!rw.get())
	{
		//找不到响应的插件读取文件
		std::cerr << "Error:could not find a suitable reader/writer to load the specified file" << std::endl;
		return 1;
	}
	//创建一个文件读取进度对象
	std::auto_ptr<progbuf> pb(new progbuf(osgDB::findDataFile(filename)));
	if (pb->is_open())
	{
		//打开文件失败
		std::cerr << "Error:could not open file" << filename << "" << std::endl;
		return 1;
	}
	std::cout << "读取进度：" << std::endl;
	//初始化一个文件输入流
	std::istream mis(pb.get());
	//读取节点
	osgDB::ReaderWriter::ReadResult rr = rw->readNode(mis);
	//得到节点
	osg::ref_ptr<osg::Node> node = rr.getNode();
	if (!node)
	{
		//读取失败
		std::cerr << "Error:could not open file" << filename << "" << std::endl;
	}
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //文件读取进度
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osgDB::Registry::instance()->addFileExtensionAlias("VR", "VR");
	//创建场景组节点
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.VR");
	root->addChild(node);
	//保存当前场景
	osgDB::writeNodeFile(*node, "scene.VR");
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //自定义文件格式读写插件
#if 0

#endif //读写中文文件名及中文路径问题
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node1 = new osg::Node;
	/*
	很关键的一个函数，用于识别unicode中文字符
	*/
	setlocale(LC_ALL, "chs");
	node1 = osgDB::readNodeFile("牛.ive");
	osg::ref_ptr<osg::Node> node2 = osgDB::readNodeFile("cessna.osg");

	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	pat->addChild(node2);
	pat->setPosition(osg::Vec3(10.0f, 0.0f, 0.0f));
	root->addChild(node1);
	root->addChild(pat);
	osgDB::writeNodeFile(*root, "牛和飞机.ive");
	viewer->setSceneData(root);
	viewer->run();
	return 0;



}