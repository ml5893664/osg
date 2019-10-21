#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/StateSetManipulator>

#include <osgUtil/Optimizer>
#include <osgUtil//Tessellator>
#include <osgUtil/Simplifier>
#include <osgUtil//DelaunayTriangulator>
#include <osgUtil/TriStripVisitor>
#include <osgUtil/SmoothingVisitor>

//����һ���ı��νڵ�4.2.2
osg::ref_ptr<osg::Node> createQuad()
{
	//����һ��Ҷ�ڵ����
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//����һ�����������
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//�����������飬ע�ⶥ������˳������ʱ���
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	//�������
	v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	//���ö�������
	geom->setVertexArray(v);
	//������������
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	//�������
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));
	//������������
	geom->setTexCoordArray(0, vt);
	//������ɫ����
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array;
	//�������
	vc->push_back(osg::Vec4(1.0f, 0.0f,0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//������ɫ����
	geom->setColorArray(vc);
	//������ɫ�İ󶨷�ʽΪ��������
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//������������
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array;
	//��ӷ���
	nc->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	//���÷�������
	geom->setNormalArray(nc);
	//���÷��ߵİ󶨷�ʽΪȫ������
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//���ͼԪ�����ƻ�ԪΪ�ı���
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	//��ӵ�Ҷ�ڵ�
	geode->addChild(geom);
	return geode;
}
//����һ���ı���4.2.3
osg::ref_ptr<osg::Node> CreateQuad1()
{
	//����һ��Ҷ�ڵ����
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//����һ��������
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//������������
	osg::ref_ptr<osg::Vec3Array> v=new osg::Vec3Array;
	//�������
	v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	//���ö�������
	geom->setVertexArray(v);
	//�����ı��ζ����������飬ָ����ͼ��ԪΪ�ı��Σ�ע�����˳��
	osg::ref_ptr<osg::DrawElementsUInt> quad = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	//�������
	quad->push_back(0);
	quad->push_back(1);
	quad->push_back(2);
	quad->push_back(3);
	//��ӵ�������
	geom->addPrimitiveSet(quad);
	//���������ζ����������飬ָ����ͼ��ԪΪ�����Σ�ע�����˳��
	osg::ref_ptr<osg::DrawElementsUInt> triangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
	//�������
	triangle->push_back(4);
	triangle->push_back(0);
	triangle->push_back(3);
	//��ӵ�������
	geom->addPrimitiveSet(triangle);
	//������ɫ����
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array;
	//�������
	vc->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//������ɫ��������
	osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 4>* colorIndex = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 4>();
	//������ݣ�ע���������˳���붥��һһ��Ӧ
	colorIndex->push_back(0);
	colorIndex->push_back(1);
	colorIndex->push_back(2);
	colorIndex->push_back(3);
	colorIndex->push_back(2);
	//������ɫ����
	geom->setColorArray(vc);
	//������ɫ��������
	geom->setSecondaryColorArray(colorIndex);
	//������ɫ�İ󶨷�ʽΪ��������
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//������������
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array;
	//��ӷ���
	nc->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	//���÷�������
	geom->setNormalArray(nc);
	//���÷��ߵİ󶨷�ʽΪȫ������
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//��ӵ�Ҷ�ڵ�
	geode->addChild(geom);
	return geode;
}
//�������Ԥ����ļ�����
osg::ref_ptr<osg::Geode> createShape()
{
	//����һ��Ҷ�ڵ�
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//���ð뾶�͸߶�
	float radius = 0.8f;
	float height = 1.0f;
	//������ϸ�ȶ��󣬾�ϸ��Խ�ߣ�ϸ�־�Խ��
	osg::ref_ptr<osg::TessellationHints> hints=new osg::TessellationHints;
	//���þ�ϸ��Ϊ0.5f
	hints->setDetailRatio(0.5f);
	//���һ�����壬��һ��������Ԥ���弸������󣬵ڶ����Ǿ�ϸ�ȣ�Ĭ��Ϊ0
	geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),radius),hints));
	//���һ��������
	geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(2.0f, 0.0f, 0.0f), 2*radius), hints));
	//���һ��Բ׶
	geode->addDrawable(new osg::ShapeDrawable(new osg::Cone(osg::Vec3(4.0f, 0.0f, 0.0f), radius, height), hints));
	//���һ��Բ����
	geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(6.0f, 0.0f, 0.0f), radius, height), hints));
	//���һ��̫�ղ�
	geode->addDrawable(new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(8.0f, 0.0f, 0.0f), radius, height), hints));
	return geode;
}
//����ηָ�
osg::ref_ptr<osg::Geode> tesslatorGeometry()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geode->addDrawable(geom);

	//������һЩ��������
	//ǽ
	const float wall[5][3]=
	{
		{ 2200.0f, 0.0f, 1130.0f },
		{ 2600.0f, 0.0f, 1130.0f },
		{ 2600.0f, 0.0f, 1340.0f },
		{ 2400.0f, 0.0f, 1440.0f },
		{ 2200.0f, 0.0f, 1340.0f }
	};
	//��
	const float door[4][3] =
	{
		{ 2360.0f, 0.0f, 1130.0f },
		{ 2440.0f, 0.0f, 1130.0f },
		{ 2440.0f, 0.0f, 1230.0f },
		{ 2360.0f, 0.0f, 1230.0f }
	};
	//���ȴ���
	const float windows[16][3] =
	{
		{ 2240.0f, 0.0f, 1180.0f },
		{ 2330.0f, 0.0f, 1180.0f },
		{ 2330.0f, 0.0f, 1220.0f },
		{ 2240.0f, 0.0f, 1220.0f },
		{ 2460.0f, 0.0f, 1180.0f },
		{ 2560.0f, 0.0f, 1180.0f },
		{ 2560.0f, 0.0f, 1220.0f },
		{ 2460.0f, 0.0f, 1220.0f },
		{ 2240.0f, 0.0f, 1280.0f },
		{ 2330.0f, 0.0f, 1280.0f },
		{ 2330.0f, 0.0f, 1320.0f },
		{ 2240.0f, 0.0f, 1320.0f },
		{ 2460.0f, 0.0f, 1280.0f },
		{ 2560.0f, 0.0f, 1280.0f },
		{ 2560.0f, 0.0f, 1320.0f },
		{ 2460.0f, 0.0f, 1320.0f }
	};
	//���ö�������
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	geom->setVertexArray(coords);
	//���÷���
	osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;
	normal->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	geom->setNormalArray(normal);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//���ǽ
	for (int i = 0; i < 5; i++)
	{
		coords->push_back(osg::Vec3(wall[i][0], wall[i][1], wall[i][2]));
	}
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, 5));
	//�����
	for (int i = 0; i < 4; i++)
	{
		coords->push_back(osg::Vec3(door[i][0], door[i][1], door[i][2]));
	}
	//��Ӵ�
	for (int i = 0; i < 16; i++)
	{
		coords->push_back(osg::Vec3(windows[i][0], windows[i][1], windows[i][2]));
	}
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 5, 20));
	//�����ָ񻯶���
	osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator;
	//���÷ָ�����Ϊ������
	tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	//����ֻ��ʾ������Ϊfalse�����ﻹ��Ҫ���
	tscx->setBoundaryOnly(false);
	//���û��ƹ���
	tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
	//ʹ�÷ָ�
	tscx->retessellatePolygons(*geom);
	return geode;
}
//����һ���ı��νڵ�
osg::ref_ptr<osg::Geometry> createQuad2()
{
	//����һ��Ҷ�ڵ����
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//����һ�����������
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//�����������飬ע�ⶥ������˳������ʱ���
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	//�������
	v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	//���ö�������
	geom->setVertexArray(v);
	//������������
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	//�������
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 1.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));
	//������������
	geom->setTexCoordArray(0, vt);
	//������ɫ����
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array;
	//�������
	vc->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//������ɫ����
	geom->setColorArray(vc);
	//������ɫ�İ󶨷�ʽΪ��������
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//������������
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array;
	//��ӷ���
	nc->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	//���÷�������
	geom->setNormalArray(nc);
	//���÷��ߵİ󶨷�ʽΪȫ������
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//���ͼԪ�����ƻ�ԪΪ�ı���
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	return geom;
}
//����һ���ı��νڵ�
osg::ref_ptr<osg::Geometry> createQuad3()
{
	//����һ��Ҷ�ڵ����
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//����һ�����������
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//�����������飬ע�ⶥ������˳������ʱ���
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	//�������
	v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	//���ö�������
	geom->setVertexArray(v);
	//������������
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	//�������
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 1.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));
	//������������
	geom->setTexCoordArray(0, vt);
	//������ɫ����
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array;
	//�������
	vc->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//������ɫ����
	geom->setColorArray(vc);
	//������ɫ�İ󶨷�ʽΪ��������
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//���ͼԪ�����ƻ�ԪΪ�ı���
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	return geom;
}

int main()
{
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createQuad());
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif	//4.2.2 �������������ʾ��

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	viewer->setSceneData(CreateQuad1());
	osgUtil::Optimizer optimizer;
	optimizer.optimize(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif	//4.2.3 �����󶨼��������ʾ��

#if 0 
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createShape());
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif	//4.3.4Ԥ���弸����ʾ��

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ӻ��ƵĶ����
	osg::ref_ptr<osg::Geode> geode = tesslatorGeometry();
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //����ηָ�

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//�л�����ģʽ������Ƚ�ģ��
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//�����������ʣ���������Խ�󣬼�Խ�٣���������ԽС����Խ��
	float sampleRatio = 0.3f;
	//���õ��������
	float maxError = 4.0f;
	//�����򻯶���
	osgUtil::Simplifier simplifier(sampleRatio, maxError);
	//��ȡţ��ģ��
	osg::ref_ptr<osg::Node> node1 = osgDB::readNodeFile("cow.osg");
	//���ţ��ģ�͵�node2�ڵ�
	osg::ref_ptr<osg::Node> node2 = (osg::Node*)(node1->clone(osg::CopyOp::DEEP_COPY_ALL));
	//����һ��λ�ñ任�ڵ�
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	//����λ��
	pat->setPosition(osg::Vec3(10.0f, 0.0f, 0.0f));
	//����ӽڵ�
	pat->addChild(node2);
	//�򻯴���
	pat->accept(simplifier);

	root->addChild(node1);
	root->addChild(pat);

	viewer->setSceneData(root);

	viewer->realize();
	viewer->run();
	return 0;
#endif //�򻯼�����ʾ��

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//����鿴�ڶ����֮���л����Բ鿴������
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));

	osg::ref_ptr<osg::Group> root = new osg::Group();

	//������������
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	//���㶥������Ĵ�С
	unsigned int n = sizeof(vertex) / sizeof(float[3]);
	//��Ӷ�������
	for (unsigned int i = 0; i < n; i++)
	{
		coords->push_back(osg::Vec3(vertex[i][0], vertex[i][1], vertex[i][2]));
	}
	//����Delaunay����������
	osg::ref_ptr<osgUtil::DelaunayTriangulator> dt = new osgUtil::DelaunayTriangulator(coords);
	//����������
	dt->triangulate();
	//����������
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	//���ö�������
	geometry->setVertexArray(coords);
	//���뵽��ͼ��Ԫ
	geometry->addPrimitiveSet(dt->getTriangles());
	//��ӵ�Ҷ�ڵ�
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geometry);
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //Delaunay����������

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//����鿴�ڶ����֮���л����Բ鿴������
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ�����������
	osg::ref_ptr<osg::Geometry> geometry = createQuad2();
	//�Լ��������������
	osgUtil::TriStripVisitor stripper;
	stripper.stripify(*geometry);
	//��ӵ�Ҷ�ڵ�
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geometry);
	//��ӵ�����
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //���Ǵ�����

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ�����������ע�⣬�����岢û��ָ�������������ע���������ɶ��㷨�ߵĴ��룬�Ϳ��Կ������յĲ��
	osg::ref_ptr<osg::Geometry> geometry = createQuad3();
	//���ɶ��㷨��
	osgUtil::SmoothingVisitor::smooth(*geometry);
	//��ӵ�Ҷ�ڵ�
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geometry);
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //���ɶ��㷨����

}

