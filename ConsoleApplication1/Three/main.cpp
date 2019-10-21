#include <osgViewer/Viewer>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Billboard>
#include <osg/Texture2D>
#include <osg/Image>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/AutoTransform>


#include <osgText/Text>

#include <osgDB//ReadFile>
#include <osgDB/WriteFile>

#include <osgUtil/Optimizer>


#include <osgsim/Impostor>

osg::ref_ptr<osg::Node> createBillboardTree(osg::ref_ptr<osg::Image> image)
{
	//创建四边形
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	//设置法线
	osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;

	//设置顶点
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	v->push_back(osg::Vec3(-0.5f, 0.0f, -0.5f));
	v->push_back(osg::Vec3(0.5f, 0.0f, -0.5f));
	v->push_back(osg::Vec3(0.5f, 0.0f, 0.5f));
	v->push_back(osg::Vec3(-0.5f, 0.0f, 0.5f));

	geometry->setVertexArray(v);
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
	//设置旋转模式为绕轴转，因此还需要设置旋转轴
	billboard2->setMode(osg::Billboard::AXIAL_ROT);
	//设置旋转轴
	billboard2->setAxis(osg::Vec3(0.0f, 0.0f, 1.0f));
	billboard2->addDrawable(geometry, osg::Vec3(10.0f, 0.0f, 0.0f));

	osg::ref_ptr<osg::Group> billboard = new osg::Group;
	billboard->addChild(billboard1);
	billboard->addChild(billboard2);

	return billboard;
}

osg::ref_ptr<osg::Node> createAutoTransform(osg::Vec3& position, float size, std::string& label, osg::AutoTransform::AutoRotateMode autoMode, osgText::Text::AxisAlignment axisAligment)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//字体
	std::string font("fonts/cour.ttf");
	//创建Text对象
	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	geode->addDrawable(text);

	//设置字体
	text->setFont(font);
	//设置字体的分辨率
	text->setFontResolution(128.0f, 128.0f);
	//设置字体的大小
	text->setCharacterSize(size);
	//设置对齐方式
	text->setAlignment(osgText::Text::CENTER_CENTER);
	//设置方向
	text->setAxisAlignment(axisAligment);
	//设置文字
	text->setText(label);

	//关闭光照
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	//创建自动变换节点
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	//添加子节点
	at->addChild(geode);
	//设置自动变换方式
	at->setAutoRotateMode(autoMode);
	//根据屏幕大小来缩放节点，默认为false，设置为true时，节点无法缩放
	at->setAutoScaleToScreen(false);
	//设置缩放的最大和最小比例
	at->setMinimumScale(0.0f);
	at->setMaximumScale(5.0f);
	//设置位置
	at->setPosition(position);

	return at;
}

osg::ref_ptr<osg::Group> createPagedLOD()
{
	osg::ref_ptr<osg::PagedLOD> page = new osg::PagedLOD;
	page->setCenter(osg::Vec3(0.0f, 0.0f, 0.0f));
	page->setFileName(0, "cow.osg");
	page->setRange(0, 0.0f, 50.0f);

	page->setFileName(1, "glider.osg");
	page->setRange(1, 50.0f, 100.0f);

	page->setFileName(2, "cessna.osg");
	page->setRange(2, 100.0f, 200.0f);

	return page;
}

//创建遮挡节点
osg::ref_ptr<osg::Node> createOccluder(const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3, const osg::Vec3& v4)
{
	//创建遮挡节点对象
	osg::ref_ptr<osg::OccluderNode> occluderNode = new osg::OccluderNode;
	//创建遮挡平面
	osg::ref_ptr<osg::ConvexPlanarOccluder> cpo = new osg::ConvexPlanarOccluder;
	//关联遮挡板平面
	occluderNode->setOccluder(cpo);
	occluderNode->setName("occluder");
	//初始化一个遮挡平面
	osg::ConvexPlanarPolygon& occluder = cpo->getOccluder();
	occluder.add(v1);
	occluder.add(v2);
	occluder.add(v3);
	occluder.add(v4);
	//为遮挡面画一个四边形
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array(occluder.getVertexList().begin(), occluder.getVertexList().end());
	geom->setVertexArray(coords);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0f, 1.0f, 1.0f, 0.5f);
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom);

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//关闭光照
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//使用混合，以保证Alpha纹理正确
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	//设置透明渲染元
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	geom->setStateSet(stateset);

	//添加四边形作为遮挡节点，遮挡节点本身不具备遮挡能力
	occluderNode->addChild(geode);

	return occluderNode;
}

//创建绕模型的遮挡场景
osg::ref_ptr<osg::Group> createOccludersAroundModel(osg::ref_ptr<osg::Node> model)
{
	//创建场景组节点
	osg::ref_ptr<osg::Group> scene = new osg::Group();
	scene->setName("OccluderScene");

	//添加子节点
	scene->addChild(model);
	model->setName("cow.osg");

	//计算模型的包围盒
	const osg::BoundingSphere bs = model->getBound();

	//根据包围盒创建前后左右四个遮挡面
	osg::BoundingBox bb;
	bb.expandBy(bs);

	//前遮挡面
	scene->addChild(createOccluder(bb.corner(0), bb.corner(1), bb.corner(5), bb.corner(4)));
	//右遮挡面
	scene->addChild(createOccluder(bb.corner(1), bb.corner(3), bb.corner(7), bb.corner(5)));
	//左遮挡面
	scene->addChild(createOccluder(bb.corner(2), bb.corner(0), bb.corner(4), bb.corner(6)));
	//后遮挡面
	scene->addChild(createOccluder(bb.corner(3), bb.corner(2), bb.corner(6), bb.corner(7)));

	return scene;
}

osg::ref_ptr<osg::Node> createEarth()
{
	osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0, 0.0, 0.0), osg::WGS_84_RADIUS_POLAR));
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(sd);

	std::string filename("Images/land_shallow_topo_2048.jpg");
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::Texture2D(osgDB::readImageFile(filename)));

	osg::ref_ptr<osg::CoordinateSystemNode> csn = new osg::CoordinateSystemNode;
	csn->setEllipsoidModel(new osg::EllipsoidModel);
	csn->addChild(geode);

	return csn;
}

int main()
{
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/tree0.rgba");
	//缩放，达到合适的大小
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	pat->setScale(osg::Vec3(5.0f, 5.0f, 5.0f));
	pat->addChild(createBillboardTree(image));
	root->addChild(pat);
	root->addChild(osgDB::readNodeFile("cow.osg"));
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif 	//3.2.2 布告板示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//创建位置变换节点pat1
	osg::ref_ptr<osg::PositionAttitudeTransform> pat1 = new osg::PositionAttitudeTransform;
	//设置位置为osg::Vec3(-10.0f,0.0f,0.0f)
	pat1->setPosition(osg::Vec3(-10.0f, 0.0f, 0.0f));
	//设置缩放，在X,Y,Z方向都缩小一倍
	pat1->setScale(osg::Vec3(0.5f, 0.5f, 0.5f));
	pat1->addChild(node);
	//创建位置变换节点pat2
	osg::ref_ptr<osg::PositionAttitudeTransform> pat2 = new osg::PositionAttitudeTransform;
	//设置位置为osg::Vec3(10.0f,0.0f,0.0f)
	pat2->setPosition(osg::Vec3(10.0f, 0.0f, 0.0f));
	//添加子节点
	pat2->addChild(node);
	//添加到场景
	root->addChild(pat1);
	root->addChild(pat2);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif	//3.3.2 位置变换节点示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<osg::MatrixTransform> mt1 = new osg::MatrixTransform;
	osg::Matrix m;
	m.makeTranslate(osg::Vec3(10.0f, 0.0f, 0.0f));
	m.makeRotate(45.0f, 1.0f, 0.0f, 0.0f);
	mt1->setMatrix(m);
	mt1->addChild(node);
	osg::ref_ptr<osg::MatrixTransform> mt2 = new osg::MatrixTransform;
	osg::Matrix t;
	t.makeTranslate(osg::Vec3(10.0f, 0.0f, 0.0f));
	mt2->setMatrix(t);
	mt2->addChild(node);
	root->addChild(mt1);
	root->addChild(mt2);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif	//3.3.4 矩阵变换节点示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	std::string text("Fly To Sky");
	root->addChild(createAutoTransform(osg::Vec3(0.0f, 0.0f, 0.0f), 60.0f, text, osg::AutoTransform::ROTATE_TO_SCREEN, osgText::Text::XY_PLANE));
	root->addChild(createAutoTransform(osg::Vec3(0.0f, 0.0f, 0.0f), 60.0f, text, osg::AutoTransform::NO_ROTATION, osgText::Text::YZ_PLANE));
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //自动对齐节点示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node1 = new osg::Node;
	node1 = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<osg::Node> node2 = osgDB::readNodeFile("glider.osg");
	osg::ref_ptr<osg::Switch> swtich = new osg::Switch;
	swtich->addChild(node1, false);
	swtich->addChild(node2, true);
	root->addChild(swtich);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //3.3.8开关节点示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node1 = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<osg::Node> node2 = osgDB::readNodeFile("glider.osg");
	osg::ref_ptr<osg::LOD> lode = new osg::LOD;
	lode->addChild(node1, 0.0f, 30.0f);
	lode->addChild(node2, 30.0f, 100.0f);
	osgDB::writeNodeFile(*lode, "lode.osg");
	root->addChild(lode);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //细节层次节点示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	mt->addChild(createPagedLOD());
	osg::Matrix m;
	m.makeRotate(60.0f, 0.0f, 1.0f);
	mt->setMatrix(m);
	root->addChild(mt);
	osgDB::writeNodeFile(*root, "Page.osg");
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //3.3.12分页细节层次节点示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<osgSim::Impostor> impostor = new osgSim::Impostor;
	impostor->setImpostorThreshold(50.0f);
	impostor->addChild(node, 0, 10000.0f);
	root->addChild(impostor);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //替代节点示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(createOccludersAroundModel(node));
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //遮挡裁剪节点示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createEarth());
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //坐标系节点示例

}