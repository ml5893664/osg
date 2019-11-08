#include <osgViewer/Viewer>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Fog>
#include <osg/Sequence>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/NodeCallback>
#include <osg/Camera>
#include <osg/ShapeDrawable>
#include <osg/ComputeBoundsVisitor>
#include <osg/BoundingBox>
#include <osg/BoundingSphere>
#include <osg/AnimationPath>

#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>

#include <osgParticle/PrecipitationEffect>
#include <osgParticle/ExplosionEffect>
#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/SmokeEffect>
#include <osgParticle/FireEffect>
#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>

#include <osgUtil/Optimizer>

#include <iostream>

//������Ч
osg::ref_ptr<osg::Fog> createFog(bool m_Linear)
{
	//����Fog����
	osg::ref_ptr<osg::Fog> fog = new osg::Fog;
	//������ɫ
	fog->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//����Ũ��
	fog->setDensity(0.01f);
	//������ЧģʽΪ������
	if (!m_Linear)
	{
		fog->setMode(osg::Fog::LINEAR);
	}
	//������ЧģʽΪȫ����
	else
	{
		fog->setMode(osg::Fog::EXP);
	}
	//������Ч����Ũ��
	fog->setStart(5.0f);
	//������ЧԶ��Ũ��
	fog->setEnd(2000.0f);
	return fog;
}
//������ըЧ��
osg::ref_ptr<osg::Node> createExplode()
{
	osg::ref_ptr<osg::Group> explode = new osg::Group;
	//����
	osg::Vec3 wind(1.0f, 0.0f, 0.0f);
	//λ��
	osg::Vec3 position(0.0f, 0.0f, -10.0f);
	//��ըģ�⣬10.0fΪ���űȣ�Ĭ��Ϊ1.0f��������
	osg::ref_ptr<osgParticle::ExplosionEffect> explosion=new osgParticle::ExplosionEffect(position,10.0f);
	//��Ƭģ��
	osg::ref_ptr<osgParticle::ExplosionDebrisEffect> explosionDebri = new osgParticle::ExplosionDebrisEffect(position, 10.0f);
	//��ģ��
	osg::ref_ptr<osgParticle::SmokeEffect> smoke = new osgParticle::SmokeEffect(position, 10.0f);
	//����ģ��
	osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(position, 10.0f);
	//���÷���
	explosion->setWind(wind);
	explosionDebri->setWind(wind);
	smoke->setWind(wind);
	fire->setWind(wind);
	//����ӽڵ�
	explode->addChild(explosion);
	explode->addChild(explosionDebri);
	explode->addChild(smoke);
	explode->addChild(fire);
	return explode;
}
//�����Զ�������ϵͳ
osg::ref_ptr<osg::Group> CreateMyParticleScene()
{
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��������ϵͳģ��
	osgParticle::Particle ptemplate;
	//������������
	ptemplate.setLifeTime(2);
	//�������Ӵ�С�仯��Χ
	ptemplate.setSizeRange(osgParticle::rangef(0.75f, 3.0f));
	//��������Alpha�仯��Χ
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f, 1.0f));
	//����������ɫ�仯��Χ
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(1.0f, 0.5f, 0.3f, 1.0f), osg::Vec4(0.0f, 0.7f, 1.0f, 0.0f)));
	//���ð뾶
	ptemplate.setRadius(0.05f);
	//��������
	ptemplate.setMass(0.05f);
	//��������ϵͳ
	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem;
	//���ò��ʣ��Ƿ�������ӣ��Ƿ���ӹ���
	ps->setDefaultAttributes("Images/smoke.rgb", false, false);
	//����ģ��
	ps->setDefaultParticleTemplate(ptemplate);
	//�������ӷ��������������������������ͷ�������
	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter;
	//��������ϵͳ
	emitter->setParticleSystem(ps);
	//�����������ͼ�����������ÿһ֡���ӵ����ӵ���Ŀ
	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter;
	//����ÿ����ӵ����ӵĸ���
	counter->setRateRange(100.0f, 100.0f);
	//����������
	emitter->setCounter(counter);
	//����һ���������
	osg::ref_ptr<osgParticle::PointPlacer> placer = new osgParticle::PointPlacer;
	//����λ��
	placer->setCenter(osg::Vec3(0.0f, 0.0f, 0.0f));
	//�����������
	emitter->setPlacer(placer);
	//�������ȷ�����
	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter;
	//���÷������ٶȱ仯��Χ
	shooter->setInitialSpeedRange(100,0);
	//����������
	emitter->setShooter(shooter);
	//���뵽������
	root->addChild(emitter);
	//������׼��̶��󣬿������������������еĸ���
	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram;
	//��������ϵͳ
	program->setParticleSystem(ps);
	//��������ģ�����
	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator;
	//�����������ٶȣ�Ĭ��Ϊ9.80665f
	ap->setToGravity(-1.0f);
	//��������
	program->addOperator(ap);
	//������������ģ��
	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator;
	//���ÿ�������,FluidViscosityΪ1.8e-5f,FluidDensityΪ1.2929f
	ffo->setFluidToAir();
	//������������
	program->addOperator(ffo);
	//��ӵ�����
	root->addChild(program);
	//��Ӹ�������ʵ��ÿ֡���ӵĹ���
	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater;
	//��������ϵͳ
	psu->addParticleSystem(ps);
	//���볡��
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(ps);
	root->addChild(geode);
	root->addChild(psu);
	return root;
}
osg::ref_ptr<osg::Node> createBillboard(osg::ref_ptr<osg::Image> image)
{
	//�����ı���
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

	//���ö���
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	v->push_back(osg::Vec3(-0.5f, 0.0f, -0.5f));
	v->push_back(osg::Vec3(0.5f, 0.0f, -0.5f));
	v->push_back(osg::Vec3(0.5f, 0.0f, 0.5f));
	v->push_back(osg::Vec3(-0.5f, 0.0f, 0.5f));

	geometry->setVertexArray(v);

	//���÷���
	osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;
	normal->push_back(osg::Vec3(1.0f, 0.0f, 0.0f) ^ osg::Vec3(0.0f, 0.0f, 1.0f));
	geometry->setNormalArray(normal);
	geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

	//������������
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 1.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));

	geometry->setTexCoordArray(0, vt);

	//�����ı���
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	if (image)
	{
		//״̬���Զ���
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		//����һ��Textture2D���Զ���
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		//����image
		texture->setImage(image);
		//����Texture2D������󣬵���������Ĭ��ΪON
		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
		//���û��
		stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		//�رչ���
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

		geometry->setStateSet(stateset);
	}

	//����Billboard����һ
	osg::ref_ptr<osg::Billboard> billboard1 = new osg::Billboard;
	//������תģʽΪ���ӵ�
	billboard1->setMode(osg::Billboard::POINT_ROT_EYE);
	//���Drawable����������λ�ã�Ĭ��λ��Ϊosg::Vec3(0.0f,0.0f,0.0f)
	billboard1->addDrawable(geometry, osg::Vec3(5.0f, 0.0f, 0.0f));

	osg::ref_ptr<osg::Billboard> billboard2 = new osg::Billboard;

	osg::ref_ptr<osg::Group> billboard = new osg::Group;
	billboard->addChild(billboard1);

	return billboard;
}
//����֡����
osg::ref_ptr<osg::Sequence> createSequence()
{
	//����֡��������
	osg::ref_ptr<osg::Sequence> seq = new osg::Sequence;
	//�ļ�����������
	typedef std::vector<std::string> Filenames;
	Filenames filenames;
	for (int i = 0; i < 60;i++)
	{
		char name_count[10];
		sprintf(name_count, "bz%d.jpg",i);
		filenames.push_back(name_count);
	}
	for (Filenames::iterator itr = filenames.begin(); itr != filenames.end();++itr)
	{
		//����ģ��
		osg::Image* image = osgDB::readImageFile(*itr);
		if (image)
		{
			//����ӽڵ�
			seq->addChild(createBillboard(image));
			//�趨�ڵ�ĳ���ʱ��
			seq->setTime(seq->getNumChildren() - 1, 1.0f);
		}
	}
	//����֡����������ʱ��
	seq->setInterval(osg::Sequence::LOOP, 0, -1);
	//���ò��ŵ��ٶȼ��ظ��Ĵ���
	seq->setDuration(1.0f, -1);
	//��ʼ����
	seq->setMode(osg::Sequence::START);
	return seq;
}
//����һ���µı任��
class ParticleTransform:public osg::MatrixTransform
{
public:
	class ParticleTransformCallback :public osg::NodeCallback
	{
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
		{
			//�õ��ڵ�
			if (ParticleTransform* ps=dynamic_cast<ParticleTransform*>(node))
			{
				osg::NodePath& fullNodePath = nv->getNodePath();
				fullNodePath.pop_back();
				//��ת���־���任
				osg::Matrix localCoordMat = osg::computeLocalToWorld(fullNodePath);
				osg::Matrix inverseOfAccum = osg::Matrix::inverse(localCoordMat);
				//���þ���
				ps->setMatrix(inverseOfAccum);
			}
			traverse(node, nv);
		}
	};
	ParticleTransform()
	{
		//���ø��»ص�
		setUpdateCallback(new ParticleTransformCallback());
	}
};
//���»ص�
class orbit:public osg::NodeCallback
{
public:
	//���캯��
	orbit() :_angle(0.0){}
	//���ؾֲ�����
	osg::Matrix getWCMatrix(){ return m; }
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::ref_ptr<osg::MatrixTransform> tx = dynamic_cast<osg::MatrixTransform*>(node);
		if (tx!=NULL)
		{
			_angle += 0.01;
			//���þ�����ת
			tx->setMatrix(osg::Matrix::rotate(_angle, 0.0f, 0.0f, 1.0f));
			//�������������굽�ֲ�����
			m = osg::computeWorldToLocal(nv->getNodePath());
		}
		traverse(node, nv);
	}
private:
	//����
	osg::Matrix m;
	//�Ƕ�
	float _angle;
};
//��ʶ��Ӱ���ն���
const int ReceivesShadowTraversalMask = 0x1;
//��ʶ��ӰͶӰ����
const int CastsShadowTraversalMask = 0x2;
//��������·��
osg::ref_ptr<osg::AnimationPath> createAnimationPath(const osg::Vec3& center, float radius, float looptime)
{
	//����һ��Path����
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::LOOP);
	int numSamples = 40;
	float yaw = 0.0f;
	float yaw_delta = 2.0f*osg::PI / (float)(numSamples - 1.0f);
	float roll = osg::inDegrees(30.0f);

	double time = 0.0f;
	double time_delta = looptime / ((double)numSamples);
	for (int i = 0; i < numSamples; i++)
	{
		osg::Vec3 position(center + osg::Vec3(sinf(yaw)*radius, cosf(yaw)*radius, 0.0f));
		osg::Quat rotation(osg::Quat(roll, osg::Vec3(0.0, 1.0, 0.0))*osg::Quat(-(yaw + osg::inDegrees(90.0f)), osg::Vec3(0.0, 0.0, 1.0)));
		animationPath->insert(time, osg::AnimationPath::ControlPoint(position, rotation));
		yaw += yaw_delta;
		time += time_delta;
	}
	return animationPath;
}
//��������ƽ��
osg::ref_ptr<osg::Node> createBase(const osg::Vec3& center, float radius)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//������������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/lz.rgb");
	if (image)
	{
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	}
	geode->setStateSet(stateset);
	//����һ���߳���
	osg::ref_ptr<osg::HeightField> grid = new osg::HeightField;
	grid->allocate(38, 39);
	grid->setOrigin(center + osg::Vec3(-radius, -radius, 0.0f));
	grid->setXInterval(radius*2.0f / (float)(38 - 1));
	grid->setYInterval(radius*2.0f / (float)(39 - 1));
	//�õ����ֵ�����ֵ����ȷ�����ʵĸ߳����ź͸߳�ƫ��
	float minHeight = FLT_MAX;
	float maxHeight = -FLT_MAX;
	unsigned int r;
	for (r = 0; r < 39;++r)
	{
		for (unsigned int c = 0; c < 38;++c)
		{
			float h = vertex[r + c * 39][2];
			if (h>maxHeight)maxHeight = h;
			if (h<minHeight)minHeight = h;
		}
	}
	float heightScale = radius*0.5 / (maxHeight - minHeight);
	float heightOffset = -(minHeight - maxHeight)*0.5f;
	//������������
	for (r = 0; r < 39;++r)
	{
		for (unsigned int c = 0; c < 38;++c)
		{
			float h = vertex[r + c * 39][2];
			grid->setHeight(c, r, (h + heightOffset)*heightScale);
		}
	}
	geode->addDrawable(new osg::ShapeDrawable(grid));
	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode);
	return group;
}
//�����ɻ�����ģ��
osg::ref_ptr<osg::Node> createMovingMode(const osg::Vec3& center, float radius)
{
	float animationLength = 10.0f;
	osg::ref_ptr<osg::AnimationPath> animationPath = createAnimationPath(center, radius, animationLength);
	osg::ref_ptr<osg::Group> model = new osg::Group;
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
	if (cessna)
	{
		const osg::BoundingSphere& bs = cessna->getBound();
		float size = radius / bs.radius()*0.3f;
		osg::ref_ptr<osg::MatrixTransform> positioned = new osg::MatrixTransform;
		positioned->setDataVariance(osg::Object::STATIC);
		positioned->setMatrix(osg::Matrix::translate(-bs.center())*osg::Matrix::scale(size, size, size)*osg::Matrix::rotate(osg::inDegrees(180.0f), 0.0f, 0.0f, 2.0f));
		positioned->addChild(cessna);
		osg::ref_ptr<osg::MatrixTransform> xform = new osg::MatrixTransform;
		xform->setUpdateCallback(new osg::AnimationPathCallback(animationPath, 0.0f, 2.0f));
		xform->addChild(positioned);
		model->addChild(xform);
	}
	return model;
}
//������������
osg::ref_ptr<osg::Node> createModel()
{
	osg::Vec3 center(0.0f, 0.0f, 0.0f);
	float radius = 100.0f;
	osg::Vec3 lightPosition(center + osg::Vec3(0.0f, 0.0f, radius));
	osg::ref_ptr<osg::Node> shadower = createMovingMode(center, radius*0.5f);
	shadower->setNodeMask(CastsShadowTraversalMask);
	osg::ref_ptr<osg::Node> shadowed = createBase(center - osg::Vec3(0.0f, 0.0f, radius*0.25), radius);
	shadowed->setNodeMask(ReceivesShadowTraversalMask);
	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(shadowed);
	group->addChild(shadower);
	return group;
}
//����һ������
osg::ref_ptr<osg::Node> createLight(osg::ref_ptr<osg::Node> model)
{
	osg::ComputeBoundsVisitor cbbv;
	model->accept(cbbv);
	osg::BoundingBox bb = cbbv.getBoundingBox();
	osg::Vec4 lightpos;
	lightpos.set(bb.center().x(), bb.center().y(), bb.zMax() + bb.radius()*2.0f, 1.0f);
	osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
	ls->getLight()->setPosition(lightpos);
	ls->getLight()->setAmbient(osg::Vec4(0.2, 0.2, 0.2, 1.0));
	ls->getLight()->setDiffuse(osg::Vec4(0.8, 0.8, 0.8, 1.0));
	return ls;
}



int main()
{
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ȡģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	root->addChild(node);
	//������Ч
	root->getOrCreateStateSet()->setAttributeAndModes(createFog(true), osg::StateAttribute::ON);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�����Ч
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ������ϵͳ
	osg::ref_ptr<osgParticle::PrecipitationEffect> pe = new osgParticle::PrecipitationEffect;
	//����ѩЧ��Ũ��Ϊ0.5f
	pe->snow(0.5f);
	//���ñ���ɫ
	viewer->getCamera()->setClearColor(pe->getFog()->getColor());
	//��ȡģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//ʹ����Ч
	node->getOrCreateStateSet()->setAttributeAndModes(pe->getFog());
	root->addChild(node);
	root->addChild(pe);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //ѩ��Ч��
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ������ϵͳ
	osg::ref_ptr<osgParticle::PrecipitationEffect> pe = new osgParticle::PrecipitationEffect;
	//����ѩЧ��Ũ��Ϊ0.5f
	pe->rain(0.5f);
	//���ñ���ɫ
	viewer->getCamera()->setClearColor(pe->getFog()->getColor());
	//��ȡģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//ʹ����Ч
	node->getOrCreateStateSet()->setAttributeAndModes(pe->getFog());
	root->addChild(node);
	root->addChild(pe);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //���Ч��
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ӱ�ըЧ��
	root->addChild(createExplode());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //��ըģ��
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//�Զ�������ϵͳ���볡��
	root->addChild(CreateMyParticleScene());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�Զ�������ϵͳ1
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//�򳡾������֡����
	osg::ref_ptr<osg::Sequence> sq = new osg::Sequence;
	sq = createSequence();
	root->addChild(sq);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�Զ�������ϵͳ2
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> rootNode = new osg::Group;
	//��ȡ��ţģ��
	osg::ref_ptr<osg::Node> tankNode = osgDB::readNodeFile("cow.osg");
	//��ȡ����ϵͳģ��
	osg::ref_ptr<osg::Node> particleNode = osgDB::readNodeFile("myvr.osg");
	//��Ӿ���任�ڵ�
	osg::ref_ptr<osg::MatrixTransform> tankTransform = new osg::MatrixTransform;
	//���ø��»ص�
	tankTransform->setUpdateCallback(new orbit());
	//����ӽڵ�
	rootNode->addChild(tankTransform);
	tankTransform->addChild(tankNode);
	//��������ϵͳ�任�ڵ�
	ParticleTransform* my = new ParticleTransform;
	//����ӽڵ�
	my->addChild(particleNode);
	tankTransform->addChild(my);

	viewer->setSceneData(rootNode);
	viewer->run();
	return 0;
#endif //����ϵͳ�Ķ�ȡ�뱣��
#if 0

#endif //��Ӱ
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ����Ӱ�ڵ㣬����ʶ���ն����ͶӰ����
	osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene;
	shadowedScene->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
	shadowedScene->setCastsShadowTraversalMask(CastsShadowTraversalMask);
	//������Ӱ����
	osg::ref_ptr<osgShadow::ShadowTexture> st = new osgShadow::ShadowTexture;
	//������Ӱ����
	shadowedScene->setShadowTechnique(st);
	osg::ref_ptr<osg::Node> node = new osg::Node;
	node = createModel();
	//�鰸�ҳ������ݲ����ӹ�Դ
	shadowedScene->addChild(createLight(node));
	shadowedScene->addChild(node);
	root->addChild(shadowedScene);
	viewer->setSceneData(root);
	viewer->run();
	return 0;

} 