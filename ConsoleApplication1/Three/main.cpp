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
	//�����ı���
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	//���÷���
	osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;

	//���ö���
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	v->push_back(osg::Vec3(-0.5f, 0.0f, -0.5f));
	v->push_back(osg::Vec3(0.5f, 0.0f, -0.5f));
	v->push_back(osg::Vec3(0.5f, 0.0f, 0.5f));
	v->push_back(osg::Vec3(-0.5f, 0.0f, 0.5f));

	geometry->setVertexArray(v);
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
	//������תģʽΪ����ת����˻���Ҫ������ת��
	billboard2->setMode(osg::Billboard::AXIAL_ROT);
	//������ת��
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
	//����
	std::string font("fonts/cour.ttf");
	//����Text����
	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	geode->addDrawable(text);

	//��������
	text->setFont(font);
	//��������ķֱ���
	text->setFontResolution(128.0f, 128.0f);
	//��������Ĵ�С
	text->setCharacterSize(size);
	//���ö��뷽ʽ
	text->setAlignment(osgText::Text::CENTER_CENTER);
	//���÷���
	text->setAxisAlignment(axisAligment);
	//��������
	text->setText(label);

	//�رչ���
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	//�����Զ��任�ڵ�
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	//����ӽڵ�
	at->addChild(geode);
	//�����Զ��任��ʽ
	at->setAutoRotateMode(autoMode);
	//������Ļ��С�����Žڵ㣬Ĭ��Ϊfalse������Ϊtrueʱ���ڵ��޷�����
	at->setAutoScaleToScreen(false);
	//�������ŵ�������С����
	at->setMinimumScale(0.0f);
	at->setMaximumScale(5.0f);
	//����λ��
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

//�����ڵ��ڵ�
osg::ref_ptr<osg::Node> createOccluder(const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3, const osg::Vec3& v4)
{
	//�����ڵ��ڵ����
	osg::ref_ptr<osg::OccluderNode> occluderNode = new osg::OccluderNode;
	//�����ڵ�ƽ��
	osg::ref_ptr<osg::ConvexPlanarOccluder> cpo = new osg::ConvexPlanarOccluder;
	//�����ڵ���ƽ��
	occluderNode->setOccluder(cpo);
	occluderNode->setName("occluder");
	//��ʼ��һ���ڵ�ƽ��
	osg::ConvexPlanarPolygon& occluder = cpo->getOccluder();
	occluder.add(v1);
	occluder.add(v2);
	occluder.add(v3);
	occluder.add(v4);
	//Ϊ�ڵ��滭һ���ı���
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
	//�رչ���
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//ʹ�û�ϣ��Ա�֤Alpha������ȷ
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	//����͸����ȾԪ
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	geom->setStateSet(stateset);

	//����ı�����Ϊ�ڵ��ڵ㣬�ڵ��ڵ㱾���߱��ڵ�����
	occluderNode->addChild(geode);

	return occluderNode;
}

//������ģ�͵��ڵ�����
osg::ref_ptr<osg::Group> createOccludersAroundModel(osg::ref_ptr<osg::Node> model)
{
	//����������ڵ�
	osg::ref_ptr<osg::Group> scene = new osg::Group();
	scene->setName("OccluderScene");

	//����ӽڵ�
	scene->addChild(model);
	model->setName("cow.osg");

	//����ģ�͵İ�Χ��
	const osg::BoundingSphere bs = model->getBound();

	//���ݰ�Χ�д���ǰ�������ĸ��ڵ���
	osg::BoundingBox bb;
	bb.expandBy(bs);

	//ǰ�ڵ���
	scene->addChild(createOccluder(bb.corner(0), bb.corner(1), bb.corner(5), bb.corner(4)));
	//���ڵ���
	scene->addChild(createOccluder(bb.corner(1), bb.corner(3), bb.corner(7), bb.corner(5)));
	//���ڵ���
	scene->addChild(createOccluder(bb.corner(2), bb.corner(0), bb.corner(4), bb.corner(6)));
	//���ڵ���
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
	//���ţ��ﵽ���ʵĴ�С
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	pat->setScale(osg::Vec3(5.0f, 5.0f, 5.0f));
	pat->addChild(createBillboardTree(image));
	root->addChild(pat);
	root->addChild(osgDB::readNodeFile("cow.osg"));
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif 	//3.2.2 �����ʾ��

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//����λ�ñ任�ڵ�pat1
	osg::ref_ptr<osg::PositionAttitudeTransform> pat1 = new osg::PositionAttitudeTransform;
	//����λ��Ϊosg::Vec3(-10.0f,0.0f,0.0f)
	pat1->setPosition(osg::Vec3(-10.0f, 0.0f, 0.0f));
	//�������ţ���X,Y,Z������Сһ��
	pat1->setScale(osg::Vec3(0.5f, 0.5f, 0.5f));
	pat1->addChild(node);
	//����λ�ñ任�ڵ�pat2
	osg::ref_ptr<osg::PositionAttitudeTransform> pat2 = new osg::PositionAttitudeTransform;
	//����λ��Ϊosg::Vec3(10.0f,0.0f,0.0f)
	pat2->setPosition(osg::Vec3(10.0f, 0.0f, 0.0f));
	//����ӽڵ�
	pat2->addChild(node);
	//��ӵ�����
	root->addChild(pat1);
	root->addChild(pat2);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif	//3.3.2 λ�ñ任�ڵ�ʾ��

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
#endif	//3.3.4 ����任�ڵ�ʾ��

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
#endif //�Զ�����ڵ�ʾ��

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
#endif //3.3.8���ؽڵ�ʾ��

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
#endif //ϸ�ڲ�νڵ�ʾ��

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
#endif //3.3.12��ҳϸ�ڲ�νڵ�ʾ��

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
#endif //����ڵ�ʾ��

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	root->addChild(createOccludersAroundModel(node));
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //�ڵ��ü��ڵ�ʾ��

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createEarth());
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //����ϵ�ڵ�ʾ��

}