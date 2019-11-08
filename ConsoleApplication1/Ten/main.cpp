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

//·�����������¼�
class AnimationEventHandler:public osgGA::GUIEventHandler
{
public:
	AnimationEventHandler(osgViewer::Viewer &vr) :viewer(vr)
	{
		//
	}
	//�¼�����
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&)
	{
		//�����������»ص�����
		osg::ref_ptr<osg::AnimationPathCallback> animationPathCallback = new osg::AnimationPathCallback;
		osg::ref_ptr<osg::Group> group = dynamic_cast<osg::Group*>(viewer.getSceneData());
		//ȡ�ýڵ�Ķ�������
		animationPathCallback = dynamic_cast<osg::AnimationPathCallback*>(group->getChild(0)->getUpdateCallback());
		switch (ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
												 if (ea.getKey()=='p')
												 {
													 //��ͣ
													 animationPathCallback->setPause(true);
													 return true;
												 }
												 if (ea.getKey()=='s')
												 {
													 //��ʼ
													 animationPathCallback->setPause(false);
													 return true;
												 }
												 if (ea.getKey()=='r')
												 {
													 //���¿�ʼ
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

//����·��
osg::ref_ptr<osg::AnimationPath> createAnimationPath(osg::Vec3& center, float radius, float looptime)
{
	//����һ��Path����
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::LOOP);
	//�ؼ�����
	int numPoint = 60;
	//ÿ��ƫ�ƽǶ�
	float yaw = 0.0f;
	float yaw_delta = 2.0f*osg::PI / (float)(numPoint - 1.0f);
	//��б�Ƕ�
	float roll = osg::inDegrees(45.0f);
	//ʱ��ƫ��
	float time = 0.0f;
	float time_delta = looptime / ((float)numPoint);
	for (int i = 0; i < numPoint;i++)
	{
		//�ؼ���λ��
		osg::Vec3 position(center + osg::Vec3(sinf(yaw)*radius, cosf(yaw)*radius, 0.0f));
		//�ؼ���Ƕ�
		osg::Quat rotation(osg::Quat(roll, osg::Vec3(0.0, 1.0, 0.0))*osg::Quat(-(yaw + osg::inDegrees(90.0f)), osg::Vec3(0.0, 0.0, 1.0)));
		//����Path���ѹؼ�����ʱ��ѹ���γ�Path
		animationPath->insert(time, osg::AnimationPath::ControlPoint(position, rotation));
		yaw += yaw_delta;
		time += time_delta;
	}
	//����path
	return animationPath;

}

//�Խڵ�����ʵ�������
osg::ref_ptr<osg::Node> createScaledNode(osg::ref_ptr<osg::Node> node, float targetScale)
{
	//ͨ����Χ��ȷ�����ʵ�����
	const osg::BoundingSphere& bsphere = node->getBound();
	float scale = targetScale / bsphere._radius;
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	pat->setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
	pat->setScale(osg::Vec3(scale, scale, scale));
	pat->setDataVariance(osg::Object::DYNAMIC);
	pat->addChild(node);
	//���߹�һ������֤������������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = pat->getOrCreateStateSet();
	stateset->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	return pat;
}
//����֡����
osg::ref_ptr<osg::Sequence> createSequence()
{
	//����֡��������
	osg::ref_ptr<osg::Sequence> seq = new osg::Sequence;
	//�ļ�����������
	typedef std::vector<std::string> Filenames;
	Filenames filenames;
	//���ģ����
	filenames.push_back("cow.osg");
	filenames.push_back("spaceship.osg");
	filenames.push_back("dumptruck.osg");
	filenames.push_back("cessna.osg");
	filenames.push_back("glider.osg");
	for (Filenames::iterator itr = filenames.begin(); itr != filenames.end();++itr)
	{
		//����ģ��
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(*itr);
		if (node)
		{
			//����ӽڵ�
			seq->addChild(createScaledNode(node, 100.0f));
			//�趨�ڵ�ĳ���ʱ��
			seq->setTime(seq->getNumChildren() - 1, 1.0f);
		}
	}
	//����֡����������ʱ��
	seq->setInterval(osg::Sequence::LOOP,0,-1);
	//���ò��ŵ��ٶȼ��ظ��Ĵ���
	seq->setDuration(1.0f,-1);
	//��ʼ����
	seq->setMode(osg::Sequence::START);
	return seq;
}
//֡�����¼�����
class SequenceEventHandler:public osgGA::GUIEventHandler
{
public:
	//���캯��
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
				//��ͣ
			case 'p':
			{
						mode = osg::Sequence::PAUSE;
						_seq->setMode(mode);
			}
				break;
				//��ʼ
			case 's':
			{
						mode = osg::Sequence::START;
						_seq->setMode(mode);
			}
				break;
				//����
			case 'r':
				{
					mode = osg::Sequence::RESUME;
					_seq->setMode(mode);
				}
				break;
				//ֹͣ
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
	//����ɻ�ģ��
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
	//�������ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//�õ���Χ�У���ȷ��������ת����
	const osg::BoundingSphere& bs = cessna->getBound();
	osg::Vec3 position = bs.center() + osg::Vec3(0.0f, 0.0f, 200.0f);
	//���ű������������������ģ�ͻῴ����
	float size = 100.0f / bs.radius()*0.3f;
	//����·��
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath = createAnimationPath(position, 200.0f, 10.0f);
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	//OSGȷ��ֻ��STATIC���ݿ��Խ���ͼ����Ⱦ
	mt->setDataVariance(osg::Object::STATIC);
	//�����ʵ��ı任��ƽ�ơ������Լ���ת��
	mt->setMatrix(osg::Matrix::translate(-bs.center())*osg::Matrix::scale(size, size, size)*osg::Matrix::rotate(osg::inDegrees(-180.0f), 0.0f, 0.0f, 1.0f));
	mt->addChild(cessna);
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	//���ø��»ص�
	pat->setUpdateCallback(new osg::AnimationPathCallback(animationPath, 0.0f, 1.0f));
	pat->addChild(mt);

	root->addChild(pat);
	root->addChild(node);

	viewer->setSceneData(root);
	//���·�����������¼�
	viewer->addEventHandler(new AnimationEventHandler(*viewer));
	viewer->run();
	return 0;
#endif //·���������Ƽ���ʾ
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����ɻ�ģ��
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
	//�������ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//�õ���Χ�У���ȷ��������ת����
	const osg::BoundingSphere& bs = cessna->getBound();
	osg::Vec3 position = bs.center() + osg::Vec3(0.0f, 0.0f, 200.0f);
	//���ű������������������ģ�ͻῴ����
	float size = 100.0f / bs.radius()*0.3f;
	//����·��
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath = createAnimationPath(position, 200.0f, 10.0f);
	std::string fileName("animation.path");
	std::ofstream out(fileName.c_str());
	animationPath->write(out);

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	//OSGȷ��ֻ��STATIC���ݿ��Խ���ͼ����Ⱦ
	mt->setDataVariance(osg::Object::STATIC);
	//�����ʵ��ı任��ƽ�ơ������Լ���ת��
	mt->setMatrix(osg::Matrix::translate(-bs.center())*osg::Matrix::scale(size, size, size)*osg::Matrix::rotate(osg::inDegrees(-180.0f), 0.0f, 0.0f, 1.0f));
	mt->addChild(cessna);
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	//���ø��»ص�
	pat->setUpdateCallback(new osg::AnimationPathCallback(animationPath, 0.0f, 1.0f));
	pat->addChild(mt);

	root->addChild(pat);
	root->addChild(node);

	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //·���ĵ���
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����ɻ�ģ��
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
	//�������ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//�õ���Χ�У���ȷ��������ת����
	const osg::BoundingSphere& bs = cessna->getBound();
	osg::Vec3 position = bs.center() + osg::Vec3(0.0f, 0.0f, 200.0f);
	//���ű������������������ģ�ͻῴ����
	float size = 100.0f / bs.radius()*0.3f;

	std::string fileName("animation.path");
	std::ifstream fin(fileName.c_str());

	//����·��
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath->read(fin);
	fin.close();

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	//OSGȷ��ֻ��STATIC���ݿ��Խ���ͼ����Ⱦ
	mt->setDataVariance(osg::Object::STATIC);
	//�����ʵ��ı任��ƽ�ơ������Լ���ת��
	mt->setMatrix(osg::Matrix::translate(-bs.center())*osg::Matrix::scale(size, size, size)*osg::Matrix::rotate(osg::inDegrees(-180.0f), 0.0f, 0.0f, 1.0f));
	mt->addChild(cessna);
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	//���ø��»ص�
	pat->setUpdateCallback(new osg::AnimationPathCallback(animationPath, 0.0f, 1.0f));
	pat->addChild(mt);

	root->addChild(pat);
	root->addChild(node);

	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //·���ĵ���
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//�򳡾������֡����
	osg::ref_ptr<osg::Sequence> sq = new osg::Sequence;
	sq = createSequence();
	root->addChild(sq);
	//���֡���������¼�
	viewer->addEventHandler(new SequenceEventHandler(sq));
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //֡������ʾ�����
#if 0

#endif //��������osgCal

}