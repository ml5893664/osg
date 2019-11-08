#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Math>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Sequence>
#include <osg/Geometry>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/TrackballManipulator>

#include <osgUtil/Optimizer>

#include <iostream>

//路径动画控制事件
class AnimationEventHandler:public osgGA::GUIEventHandler
{
public:
	AnimationEventHandler(osgViewer::Viewer &vr) :viewer(vr)
	{
		//
	}
	//事件处理
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&)
	{
		//创建动画更新回调对象
		osg::ref_ptr<osg::AnimationPathCallback> animationPathCallback = new osg::AnimationPathCallback;
		osg::ref_ptr<osg::Group> group = dynamic_cast<osg::Group*>(viewer.getSceneData());
		//取得节点的动画属性
		animationPathCallback = dynamic_cast<osg::AnimationPathCallback*>(group->getChild(0)->getUpdateCallback());
		switch (ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
												 if (ea.getKey()=='p')
												 {
													 //暂停
													 animationPathCallback->setPause(true);
													 return true;
												 }
												 if (ea.getKey()=='s')
												 {
													 //开始
													 animationPathCallback->setPause(false);
													 return true;
												 }
												 if (ea.getKey()=='r')
												 {
													 //重新开始
													 animationPathCallback->reset();
													 return true;
												 }
												 break;
		}
		default:
			break;
		}
		return false;
	}
	osgViewer::Viewer &viewer;
protected:
private:
};

//创建路径
osg::ref_ptr<osg::AnimationPath> createAnimationPath(osg::Vec3& center, float radius, float looptime)
{
	//创建一个Path对象
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::LOOP);
	//关键点数
	int numPoint = 60;
	//每次偏移角度
	float yaw = 0.0f;
	float yaw_delta = 2.0f*osg::PI / (float)(numPoint - 1.0f);
	//倾斜角度
	float roll = osg::inDegrees(45.0f);
	//时间偏移
	float time = 0.0f;
	float time_delta = looptime / ((float)numPoint);
	for (int i = 0; i < numPoint;i++)
	{
		//关键点位置
		osg::Vec3 position(center + osg::Vec3(sinf(yaw)*radius, cosf(yaw)*radius, 0.0f));
		//关键点角度
		osg::Quat rotation(osg::Quat(roll, osg::Vec3(0.0, 1.0, 0.0))*osg::Quat(-(yaw + osg::inDegrees(90.0f)), osg::Vec3(0.0, 0.0, 1.0)));
		//插入Path，把关键点与时间压入形成Path
		animationPath->insert(time, osg::AnimationPath::ControlPoint(position, rotation));
		yaw += yaw_delta;
		time += time_delta;
	}
	//返回path
	return animationPath;

}

//对节点进行适当的缩放
osg::ref_ptr<osg::Node> createScaledNode(osg::ref_ptr<osg::Node> node, float targetScale)
{
	//通过包围盒确定合适的缩放
	const osg::BoundingSphere& bsphere = node->getBound();
	float scale = targetScale / bsphere._radius;
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	pat->setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
	pat->setScale(osg::Vec3(scale, scale, scale));
	pat->setDataVariance(osg::Object::DYNAMIC);
	pat->addChild(node);
	//法线归一化，保证光照明暗均匀
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = pat->getOrCreateStateSet();
	stateset->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	return pat;
}
//创建帧动画
osg::ref_ptr<osg::Sequence> createSequence()
{
	//创建帧动画对象
	osg::ref_ptr<osg::Sequence> seq = new osg::Sequence;
	//文件名向量对象
	typedef std::vector<std::string> Filenames;
	Filenames filenames;
	//添加模型名
	filenames.push_back("cow.osg");
	filenames.push_back("spaceship.osg");
	filenames.push_back("dumptruck.osg");
	filenames.push_back("cessna.osg");
	filenames.push_back("glider.osg");
	for (Filenames::iterator itr = filenames.begin(); itr != filenames.end();++itr)
	{
		//加载模型
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(*itr);
		if (node)
		{
			//添加子节点
			seq->addChild(createScaledNode(node, 100.0f));
			//设定节点的持续时间
			seq->setTime(seq->getNumChildren() - 1, 1.0f);
		}
	}
	//设置帧动画持续的时间
	seq->setInterval(osg::Sequence::LOOP,0,-1);
	//设置播放的速度及重复的次数
	seq->setDuration(1.0f,-1);
	//开始播放
	seq->setMode(osg::Sequence::START);
	return seq;
}
//帧动画事件控制
class SequenceEventHandler:public osgGA::GUIEventHandler
{
public:
	//构造函数
	SequenceEventHandler(osg::ref_ptr<osg::Sequence> seq)
	{
		_seq = seq;
	}
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&)
	{
		osg::Sequence::SequenceMode mode = _seq->getMode();
		if (ea.getEventType()==osgGA::GUIEventAdapter::KEYDOWN)
		{
			switch (ea.getEventType())
			{
				//暂停
			case 'p':
			{
						mode = osg::Sequence::PAUSE;
						_seq->setMode(mode);
			}
				break;
				//开始
			case 's':
			{
						mode = osg::Sequence::START;
						_seq->setMode(mode);
			}
				break;
				//继续
			case 'r':
				{
					mode = osg::Sequence::RESUME;
					_seq->setMode(mode);
				}
				break;
				//停止
			case 't':
				{
					mode = osg::Sequence::STOP;
					_seq->setMode(mode);
				}
				break;
				default:
					break;
			}
		}
		return false;
	}
private:
	osg::ref_ptr<osg::Sequence> _seq;
};

int main()
{
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读入飞机模型
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
	//读入地形模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//得到包围盒，来确定动画旋转中心
	const osg::BoundingSphere& bs = cessna->getBound();
	osg::Vec3 position = bs.center() + osg::Vec3(0.0f, 0.0f, 200.0f);
	//缩放比例，如果比例不当，模型会看不见
	float size = 100.0f / bs.radius()*0.3f;
	//创建路径
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath = createAnimationPath(position, 200.0f, 10.0f);
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	//OSG确保只有STATIC数据可以进行图形渲染
	mt->setDataVariance(osg::Object::STATIC);
	//进行适当的变换（平移、缩放以及旋转）
	mt->setMatrix(osg::Matrix::translate(-bs.center())*osg::Matrix::scale(size, size, size)*osg::Matrix::rotate(osg::inDegrees(-180.0f), 0.0f, 0.0f, 1.0f));
	mt->addChild(cessna);
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	//设置更新回调
	pat->setUpdateCallback(new osg::AnimationPathCallback(animationPath, 0.0f, 1.0f));
	pat->addChild(mt);

	root->addChild(pat);
	root->addChild(node);

	viewer->setSceneData(root);
	//添加路径动画控制事件
	viewer->addEventHandler(new AnimationEventHandler(*viewer));
	viewer->run();
	return 0;
#endif //路径动画控制及显示
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读入飞机模型
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
	//读入地形模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//得到包围盒，来确定动画旋转中心
	const osg::BoundingSphere& bs = cessna->getBound();
	osg::Vec3 position = bs.center() + osg::Vec3(0.0f, 0.0f, 200.0f);
	//缩放比例，如果比例不当，模型会看不见
	float size = 100.0f / bs.radius()*0.3f;
	//创建路径
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath = createAnimationPath(position, 200.0f, 10.0f);
	std::string fileName("animation.path");
	std::ofstream out(fileName.c_str());
	animationPath->write(out);

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	//OSG确保只有STATIC数据可以进行图形渲染
	mt->setDataVariance(osg::Object::STATIC);
	//进行适当的变换（平移、缩放以及旋转）
	mt->setMatrix(osg::Matrix::translate(-bs.center())*osg::Matrix::scale(size, size, size)*osg::Matrix::rotate(osg::inDegrees(-180.0f), 0.0f, 0.0f, 1.0f));
	mt->addChild(cessna);
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	//设置更新回调
	pat->setUpdateCallback(new osg::AnimationPathCallback(animationPath, 0.0f, 1.0f));
	pat->addChild(mt);

	root->addChild(pat);
	root->addChild(node);

	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //路径的导出
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读入飞机模型
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
	//读入地形模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//得到包围盒，来确定动画旋转中心
	const osg::BoundingSphere& bs = cessna->getBound();
	osg::Vec3 position = bs.center() + osg::Vec3(0.0f, 0.0f, 200.0f);
	//缩放比例，如果比例不当，模型会看不见
	float size = 100.0f / bs.radius()*0.3f;

	std::string fileName("animation.path");
	std::ifstream fin(fileName.c_str());

	//创建路径
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath->read(fin);
	fin.close();

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	//OSG确保只有STATIC数据可以进行图形渲染
	mt->setDataVariance(osg::Object::STATIC);
	//进行适当的变换（平移、缩放以及旋转）
	mt->setMatrix(osg::Matrix::translate(-bs.center())*osg::Matrix::scale(size, size, size)*osg::Matrix::rotate(osg::inDegrees(-180.0f), 0.0f, 0.0f, 1.0f));
	mt->addChild(cessna);
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	//设置更新回调
	pat->setUpdateCallback(new osg::AnimationPathCallback(animationPath, 0.0f, 1.0f));
	pat->addChild(mt);

	root->addChild(pat);
	root->addChild(node);

	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //路径的导入
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//向场景中添加帧动画
	osg::ref_ptr<osg::Sequence> sq = new osg::Sequence;
	sq = createSequence();
	root->addChild(sq);
	//添加帧动画控制事件
	viewer->addEventHandler(new SequenceEventHandler(sq));
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //帧动画显示与控制
#if 0

#endif //骨骼动画osgCal

}