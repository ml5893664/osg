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

//创建一个四边形节点4.2.2
osg::ref_ptr<osg::Node> createQuad()
{
	//创建一个叶节点对象
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//创建一个几何体对象
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//创建顶点数组，注意顶点的添加顺序是逆时针的
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	//添加数据
	v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	//设置顶点数据
	geom->setVertexArray(v);
	//创建纹理坐标
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	//添加数据
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));
	//设置纹理坐标
	geom->setTexCoordArray(0, vt);
	//创建颜色数组
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array;
	//添加数据
	vc->push_back(osg::Vec4(1.0f, 0.0f,0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//设置颜色数组
	geom->setColorArray(vc);
	//设置颜色的绑定方式为单个顶点
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//创建法线数组
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array;
	//添加法线
	nc->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	//设置法线数组
	geom->setNormalArray(nc);
	//设置法线的绑定方式为全部顶点
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//添加图元，绘制基元为四边形
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	//添加到叶节点
	geode->addChild(geom);
	return geode;
}
//创建一个四边形4.2.3
osg::ref_ptr<osg::Node> CreateQuad1()
{
	//创建一个叶节点对象
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//创建一个几何体
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//创建顶点数组
	osg::ref_ptr<osg::Vec3Array> v=new osg::Vec3Array;
	//添加数据
	v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	//设置顶点数据
	geom->setVertexArray(v);
	//创建四边形顶点索引数组，指定绘图基元为四边形，注意添加顺序
	osg::ref_ptr<osg::DrawElementsUInt> quad = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	//添加数据
	quad->push_back(0);
	quad->push_back(1);
	quad->push_back(2);
	quad->push_back(3);
	//添加到几何体
	geom->addPrimitiveSet(quad);
	//创建三角形顶点索引数组，指定绘图基元为三角形，注意添加顺序
	osg::ref_ptr<osg::DrawElementsUInt> triangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
	//添加数据
	triangle->push_back(4);
	triangle->push_back(0);
	triangle->push_back(3);
	//添加到几何体
	geom->addPrimitiveSet(triangle);
	//创建颜色数组
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array;
	//添加数组
	vc->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//创建颜色索引数组
	osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 4>* colorIndex = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 4>();
	//添加数据，注意添加数据顺序与顶点一一对应
	colorIndex->push_back(0);
	colorIndex->push_back(1);
	colorIndex->push_back(2);
	colorIndex->push_back(3);
	colorIndex->push_back(2);
	//设置颜色数组
	geom->setColorArray(vc);
	//设置颜色索引数组
	geom->setSecondaryColorArray(colorIndex);
	//设置颜色的绑定方式为单个顶点
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//创建法线数组
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array;
	//添加法线
	nc->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	//设置法线数组
	geom->setNormalArray(nc);
	//设置法线的绑定方式为全部顶点
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//添加到叶节点
	geode->addChild(geom);
	return geode;
}
//创建多个预定义的几何体
osg::ref_ptr<osg::Geode> createShape()
{
	//创建一个叶节点
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//设置半径和高度
	float radius = 0.8f;
	float height = 1.0f;
	//创建精细度对象，精细度越高，细分就越多
	osg::ref_ptr<osg::TessellationHints> hints=new osg::TessellationHints;
	//设置精细度为0.5f
	hints->setDetailRatio(0.5f);
	//添加一个球体，第一个参数是预定义几何体对象，第二个是精细度，默认为0
	geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),radius),hints));
	//添加一个正方形
	geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(2.0f, 0.0f, 0.0f), 2*radius), hints));
	//添加一个圆锥
	geode->addDrawable(new osg::ShapeDrawable(new osg::Cone(osg::Vec3(4.0f, 0.0f, 0.0f), radius, height), hints));
	//添加一个圆柱体
	geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(6.0f, 0.0f, 0.0f), radius, height), hints));
	//添加一个太空舱
	geode->addDrawable(new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(8.0f, 0.0f, 0.0f), radius, height), hints));
	return geode;
}
//多边形分格化
osg::ref_ptr<osg::Geode> tesslatorGeometry()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geode->addDrawable(geom);

	//以下是一些顶点数据
	//墙
	const float wall[5][3]=
	{
		{ 2200.0f, 0.0f, 1130.0f },
		{ 2600.0f, 0.0f, 1130.0f },
		{ 2600.0f, 0.0f, 1340.0f },
		{ 2400.0f, 0.0f, 1440.0f },
		{ 2200.0f, 0.0f, 1340.0f }
	};
	//门
	const float door[4][3] =
	{
		{ 2360.0f, 0.0f, 1130.0f },
		{ 2440.0f, 0.0f, 1130.0f },
		{ 2440.0f, 0.0f, 1230.0f },
		{ 2360.0f, 0.0f, 1230.0f }
	};
	//四扇窗户
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
	//设置顶点数据
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	geom->setVertexArray(coords);
	//设置法线
	osg::ref_ptr<osg::Vec3Array> normal = new osg::Vec3Array;
	normal->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	geom->setNormalArray(normal);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//添加墙
	for (int i = 0; i < 5; i++)
	{
		coords->push_back(osg::Vec3(wall[i][0], wall[i][1], wall[i][2]));
	}
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, 5));
	//添加门
	for (int i = 0; i < 4; i++)
	{
		coords->push_back(osg::Vec3(door[i][0], door[i][1], door[i][2]));
	}
	//添加窗
	for (int i = 0; i < 16; i++)
	{
		coords->push_back(osg::Vec3(windows[i][0], windows[i][1], windows[i][2]));
	}
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 5, 20));
	//创建分格化对象
	osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator;
	//设置分格类型为几何体
	tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	//设置只显示轮廓线为false，这里还需要填充
	tscx->setBoundaryOnly(false);
	//设置环绕规则
	tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
	//使用分格化
	tscx->retessellatePolygons(*geom);
	return geode;
}
//创建一个四边形节点
osg::ref_ptr<osg::Geometry> createQuad2()
{
	//创建一个叶节点对象
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//创建一个几何体对象
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//创建顶点数组，注意顶点的添加顺序是逆时针的
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	//添加数据
	v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	//设置顶点数据
	geom->setVertexArray(v);
	//创建纹理坐标
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	//添加数据
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 1.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));
	//设置纹理坐标
	geom->setTexCoordArray(0, vt);
	//创建颜色数组
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array;
	//添加数据
	vc->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//设置颜色数组
	geom->setColorArray(vc);
	//设置颜色的绑定方式为单个顶点
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//创建法线数组
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array;
	//添加法线
	nc->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	//设置法线数组
	geom->setNormalArray(nc);
	//设置法线的绑定方式为全部顶点
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//添加图元，绘制基元为四边形
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	return geom;
}
//创建一个四边形节点
osg::ref_ptr<osg::Geometry> createQuad3()
{
	//创建一个叶节点对象
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//创建一个几何体对象
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//创建顶点数组，注意顶点的添加顺序是逆时针的
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	//添加数据
	v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	//设置顶点数据
	geom->setVertexArray(v);
	//创建纹理坐标
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	//添加数据
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 1.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));
	//设置纹理坐标
	geom->setTexCoordArray(0, vt);
	//创建颜色数组
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array;
	//添加数据
	vc->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//设置颜色数组
	geom->setColorArray(vc);
	//设置颜色的绑定方式为单个顶点
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//添加图元，绘制基元为四边形
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
#endif	//4.2.2 基本几何体绘制示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	viewer->setSceneData(CreateQuad1());
	osgUtil::Optimizer optimizer;
	optimizer.optimize(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif	//4.2.3 索引绑定几何体绘制示例

#if 0 
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createShape());
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif	//4.3.4预定义几何体示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//添加绘制的多边形
	osg::ref_ptr<osg::Geode> geode = tesslatorGeometry();
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //多边形分格化

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//切换网络模式，方便比较模型
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//设置样本比率，样本比率越大，简化越少；样本比率越小，简化越多
	float sampleRatio = 0.3f;
	//设置点的最大误差
	float maxError = 4.0f;
	//创建简化对象
	osgUtil::Simplifier simplifier(sampleRatio, maxError);
	//读取牛的模型
	osg::ref_ptr<osg::Node> node1 = osgDB::readNodeFile("cow.osg");
	//深拷贝牛的模型到node2节点
	osg::ref_ptr<osg::Node> node2 = (osg::Node*)(node1->clone(osg::CopyOp::DEEP_COPY_ALL));
	//创建一个位置变换节点
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	//设置位置
	pat->setPosition(osg::Vec3(10.0f, 0.0f, 0.0f));
	//添加子节点
	pat->addChild(node2);
	//简化处理
	pat->accept(simplifier);

	root->addChild(node1);
	root->addChild(pat);

	viewer->setSceneData(root);

	viewer->realize();
	viewer->run();
	return 0;
#endif //简化几何体示例

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//方便查看在多边形之间切换，以查看三角网
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));

	osg::ref_ptr<osg::Group> root = new osg::Group();

	//创建顶点数组
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
	//计算顶点数组的大小
	unsigned int n = sizeof(vertex) / sizeof(float[3]);
	//添加顶点数据
	for (unsigned int i = 0; i < n; i++)
	{
		coords->push_back(osg::Vec3(vertex[i][0], vertex[i][1], vertex[i][2]));
	}
	//创建Delaunay三角网对象
	osg::ref_ptr<osgUtil::DelaunayTriangulator> dt = new osgUtil::DelaunayTriangulator(coords);
	//生成三角网
	dt->triangulate();
	//创建几何体
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	//设置顶点数组
	geometry->setVertexArray(coords);
	//加入到绘图基元
	geometry->addPrimitiveSet(dt->getTriangles());
	//添加到叶节点
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geometry);
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //Delaunay三角网绘制

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//方便查看在多边形之间切换，以查看三角网
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建一个几何体对象
	osg::ref_ptr<osg::Geometry> geometry = createQuad2();
	//对几何体进行条带化
	osgUtil::TriStripVisitor stripper;
	stripper.stripify(*geometry);
	//添加到叶节点
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geometry);
	//添加到场景
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //三角带绘制

#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建一个几何体对象，注意，几何体并没有指定法线如果读者注释下面生成顶点法线的代码，就可以看到光照的差别
	osg::ref_ptr<osg::Geometry> geometry = createQuad3();
	//生成顶点法线
	osgUtil::SmoothingVisitor::smooth(*geometry);
	//添加到叶节点
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geometry);
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //生成顶点法向量

}

