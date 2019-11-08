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

//创建雾效
osg::ref_ptr<osg::Fog> createFog(bool m_Linear)
{
	//创建Fog对象
	osg::ref_ptr<osg::Fog> fog = new osg::Fog;
	//设置颜色
	fog->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//设置浓度
	fog->setDensity(0.01f);
	//设置雾效模式为线性雾
	if (!m_Linear)
	{
		fog->setMode(osg::Fog::LINEAR);
	}
	//设置雾效模式为全局雾
	else
	{
		fog->setMode(osg::Fog::EXP);
	}
	//设置雾效近点浓度
	fog->setStart(5.0f);
	//设置雾效远点浓度
	fog->setEnd(2000.0f);
	return fog;
}
//创建爆炸效果
osg::ref_ptr<osg::Node> createExplode()
{
	osg::ref_ptr<osg::Group> explode = new osg::Group;
	//风向
	osg::Vec3 wind(1.0f, 0.0f, 0.0f);
	//位置
	osg::Vec3 position(0.0f, 0.0f, -10.0f);
	//爆炸模拟，10.0f为缩放比，默认为1.0f，不缩放
	osg::ref_ptr<osgParticle::ExplosionEffect> explosion=new osgParticle::ExplosionEffect(position,10.0f);
	//碎片模拟
	osg::ref_ptr<osgParticle::ExplosionDebrisEffect> explosionDebri = new osgParticle::ExplosionDebrisEffect(position, 10.0f);
	//烟模拟
	osg::ref_ptr<osgParticle::SmokeEffect> smoke = new osgParticle::SmokeEffect(position, 10.0f);
	//火焰模拟
	osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(position, 10.0f);
	//设置风向
	explosion->setWind(wind);
	explosionDebri->setWind(wind);
	smoke->setWind(wind);
	fire->setWind(wind);
	//添加子节点
	explode->addChild(explosion);
	explode->addChild(explosionDebri);
	explode->addChild(smoke);
	explode->addChild(fire);
	return explode;
}
//创建自定义粒子系统
osg::ref_ptr<osg::Group> CreateMyParticleScene()
{
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建例子系统模板
	osgParticle::Particle ptemplate;
	//设置生命周期
	ptemplate.setLifeTime(2);
	//设置粒子大小变化范围
	ptemplate.setSizeRange(osgParticle::rangef(0.75f, 3.0f));
	//设置粒子Alpha变化范围
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f, 1.0f));
	//设置粒子颜色变化范围
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(1.0f, 0.5f, 0.3f, 1.0f), osg::Vec4(0.0f, 0.7f, 1.0f, 0.0f)));
	//设置半径
	ptemplate.setRadius(0.05f);
	//设置重置
	ptemplate.setMass(0.05f);
	//创建粒子系统
	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem;
	//设置材质，是否放射粒子，是否添加光照
	ps->setDefaultAttributes("Images/smoke.rgb", false, false);
	//加入模板
	ps->setDefaultParticleTemplate(ptemplate);
	//创建粒子放射器（包括计数器、放置器和发射器）
	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter;
	//关联粒子系统
	emitter->setParticleSystem(ps);
	//创建发射器和计数器，调整每一帧增加的粒子的数目
	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter;
	//设置每秒添加的粒子的个数
	counter->setRateRange(100.0f, 100.0f);
	//关联计数器
	emitter->setCounter(counter);
	//设置一个点放置器
	osg::ref_ptr<osgParticle::PointPlacer> placer = new osgParticle::PointPlacer;
	//放置位置
	placer->setCenter(osg::Vec3(0.0f, 0.0f, 0.0f));
	//关联点放置器
	emitter->setPlacer(placer);
	//创建弧度发射器
	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter;
	//设置发射器速度变化范围
	shooter->setInitialSpeedRange(100,0);
	//关联发射器
	emitter->setShooter(shooter);
	//加入到场景中
	root->addChild(emitter);
	//创建标准编程对象，控制粒子在生命周期中的更新
	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram;
	//关联粒子系统
	program->setParticleSystem(ps);
	//创建重力模拟对象
	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator;
	//设置重力加速度，默认为9.80665f
	ap->setToGravity(-1.0f);
	//关联重力
	program->addOperator(ap);
	//创建空气阻力模拟
	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator;
	//设置空气属性,FluidViscosity为1.8e-5f,FluidDensity为1.2929f
	ffo->setFluidToAir();
	//关联空气阻力
	program->addOperator(ffo);
	//添加到场景
	root->addChild(program);
	//添加更新器，实现每帧粒子的管理
	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater;
	//关联粒子系统
	psu->addParticleSystem(ps);
	//加入场景
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(ps);
	root->addChild(geode);
	root->addChild(psu);
	return root;
}
osg::ref_ptr<osg::Node> createBillboard(osg::ref_ptr<osg::Image> image)
{
	//创建四边形
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

	//设置顶点
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	v->push_back(osg::Vec3(-0.5f, 0.0f, -0.5f));
	v->push_back(osg::Vec3(0.5f, 0.0f, -0.5f));
	v->push_back(osg::Vec3(0.5f, 0.0f, 0.5f));
	v->push_back(osg::Vec3(-0.5f, 0.0f, 0.5f));

	geometry->setVertexArray(v);

	//设置法线
	osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;
	normal->push_back(osg::Vec3(1.0f, 0.0f, 0.0f) ^ osg::Vec3(0.0f, 0.0f, 1.0f));
	geometry->setNormalArray(normal);
	geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

	//设置纹理坐标
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 1.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));

	geometry->setTexCoordArray(0, vt);

	//绘制四边形
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	if (image)
	{
		//状态属性对象
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		//创建一个Textture2D属性对象
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		//关联image
		texture->setImage(image);
		//关联Texture2D纹理对象，第三个参数默认为ON
		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
		//启用混合
		stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		//关闭光照
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

		geometry->setStateSet(stateset);
	}

	//创建Billboard对象一
	osg::ref_ptr<osg::Billboard> billboard1 = new osg::Billboard;
	//设置旋转模式为绕视点
	billboard1->setMode(osg::Billboard::POINT_ROT_EYE);
	//添加Drawable，并设置其位置，默认位置为osg::Vec3(0.0f,0.0f,0.0f)
	billboard1->addDrawable(geometry, osg::Vec3(5.0f, 0.0f, 0.0f));

	osg::ref_ptr<osg::Billboard> billboard2 = new osg::Billboard;

	osg::ref_ptr<osg::Group> billboard = new osg::Group;
	billboard->addChild(billboard1);

	return billboard;
}
//创建帧动画
osg::ref_ptr<osg::Sequence> createSequence()
{
	//创建帧动画对象
	osg::ref_ptr<osg::Sequence> seq = new osg::Sequence;
	//文件名向量对象
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
		//加载模型
		osg::Image* image = osgDB::readImageFile(*itr);
		if (image)
		{
			//添加子节点
			seq->addChild(createBillboard(image));
			//设定节点的持续时间
			seq->setTime(seq->getNumChildren() - 1, 1.0f);
		}
	}
	//设置帧动画持续的时间
	seq->setInterval(osg::Sequence::LOOP, 0, -1);
	//设置播放的速度及重复的次数
	seq->setDuration(1.0f, -1);
	//开始播放
	seq->setMode(osg::Sequence::START);
	return seq;
}
//创建一个新的变换类
class ParticleTransform:public osg::MatrixTransform
{
public:
	class ParticleTransformCallback :public osg::NodeCallback
	{
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
		{
			//得到节点
			if (ParticleTransform* ps=dynamic_cast<ParticleTransform*>(node))
			{
				osg::NodePath& fullNodePath = nv->getNodePath();
				fullNodePath.pop_back();
				//反转各种矩阵变换
				osg::Matrix localCoordMat = osg::computeLocalToWorld(fullNodePath);
				osg::Matrix inverseOfAccum = osg::Matrix::inverse(localCoordMat);
				//设置矩阵
				ps->setMatrix(inverseOfAccum);
			}
			traverse(node, nv);
		}
	};
	ParticleTransform()
	{
		//设置更新回调
		setUpdateCallback(new ParticleTransformCallback());
	}
};
//更新回调
class orbit:public osg::NodeCallback
{
public:
	//构造函数
	orbit() :_angle(0.0){}
	//返回局部矩阵
	osg::Matrix getWCMatrix(){ return m; }
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::ref_ptr<osg::MatrixTransform> tx = dynamic_cast<osg::MatrixTransform*>(node);
		if (tx!=NULL)
		{
			_angle += 0.01;
			//设置矩阵旋转
			tx->setMatrix(osg::Matrix::rotate(_angle, 0.0f, 0.0f, 1.0f));
			//计算由世界坐标到局部坐标
			m = osg::computeWorldToLocal(nv->getNodePath());
		}
		traverse(node, nv);
	}
private:
	//矩阵
	osg::Matrix m;
	//角度
	float _angle;
};
//标识阴影接收对象
const int ReceivesShadowTraversalMask = 0x1;
//标识阴影投影对象
const int CastsShadowTraversalMask = 0x2;
//创建动画路径
osg::ref_ptr<osg::AnimationPath> createAnimationPath(const osg::Vec3& center, float radius, float looptime)
{
	//创建一个Path对象
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
//创建地形平面
osg::ref_ptr<osg::Node> createBase(const osg::Vec3& center, float radius)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//设置纹理属性
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/lz.rgb");
	if (image)
	{
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	}
	geode->setStateSet(stateset);
	//申请一个高程类
	osg::ref_ptr<osg::HeightField> grid = new osg::HeightField;
	grid->allocate(38, 39);
	grid->setOrigin(center + osg::Vec3(-radius, -radius, 0.0f));
	grid->setXInterval(radius*2.0f / (float)(38 - 1));
	grid->setYInterval(radius*2.0f / (float)(39 - 1));
	//得到最高值和最低值，以确定合适的高程缩放和高程偏移
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
	//设置行列数据
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
//创建飞机动画模型
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
//创建场景数据
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
//创建一个光照
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
	//读取模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	root->addChild(node);
	//启动雾效
	root->getOrCreateStateSet()->setAttributeAndModes(createFog(true), osg::StateAttribute::ON);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //雾的特效
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//申请一个粒子系统
	osg::ref_ptr<osgParticle::PrecipitationEffect> pe = new osgParticle::PrecipitationEffect;
	//设置雪效的浓度为0.5f
	pe->snow(0.5f);
	//设置背景色
	viewer->getCamera()->setClearColor(pe->getFog()->getColor());
	//读取模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//使用雾效
	node->getOrCreateStateSet()->setAttributeAndModes(pe->getFog());
	root->addChild(node);
	root->addChild(pe);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //雪的效果
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//申请一个粒子系统
	osg::ref_ptr<osgParticle::PrecipitationEffect> pe = new osgParticle::PrecipitationEffect;
	//设置雪效的浓度为0.5f
	pe->rain(0.5f);
	//设置背景色
	viewer->getCamera()->setClearColor(pe->getFog()->getColor());
	//读取模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	//使用雾效
	node->getOrCreateStateSet()->setAttributeAndModes(pe->getFog());
	root->addChild(node);
	root->addChild(pe);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //雨的效果
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//添加爆炸效果
	root->addChild(createExplode());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //爆炸模拟
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//自定义粒子系统加入场景
	root->addChild(CreateMyParticleScene());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //自定义粒子系统1
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//向场景中添加帧动画
	osg::ref_ptr<osg::Sequence> sq = new osg::Sequence;
	sq = createSequence();
	root->addChild(sq);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //自定义粒子系统2
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> rootNode = new osg::Group;
	//读取奶牛模型
	osg::ref_ptr<osg::Node> tankNode = osgDB::readNodeFile("cow.osg");
	//读取粒子系统模型
	osg::ref_ptr<osg::Node> particleNode = osgDB::readNodeFile("myvr.osg");
	//添加矩阵变换节点
	osg::ref_ptr<osg::MatrixTransform> tankTransform = new osg::MatrixTransform;
	//设置更新回调
	tankTransform->setUpdateCallback(new orbit());
	//添加子节点
	rootNode->addChild(tankTransform);
	tankTransform->addChild(tankNode);
	//创建粒子系统变换节点
	ParticleTransform* my = new ParticleTransform;
	//添加子节点
	my->addChild(particleNode);
	tankTransform->addChild(my);

	viewer->setSceneData(rootNode);
	viewer->run();
	return 0;
#endif //粒子系统的读取与保存
#if 0

#endif //阴影
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建一个阴影节点，并标识接收对象和投影对象
	osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene;
	shadowedScene->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
	shadowedScene->setCastsShadowTraversalMask(CastsShadowTraversalMask);
	//创建阴影纹理
	osg::ref_ptr<osgShadow::ShadowTexture> st = new osgShadow::ShadowTexture;
	//关联阴影纹理
	shadowedScene->setShadowTechnique(st);
	osg::ref_ptr<osg::Node> node = new osg::Node;
	node = createModel();
	//议案家场景数据并增加光源
	shadowedScene->addChild(createLight(node));
	shadowedScene->addChild(node);
	root->addChild(shadowedScene);
	viewer->setSceneData(root);
	viewer->run();
	return 0;

} 