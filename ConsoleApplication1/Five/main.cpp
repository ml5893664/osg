#include <osgViewer/Viewer>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Camera>
#include <osg/Image>
#include <osg/TexGen>
#include <osg/Texture1D>
#include <osg/TexEnv>
#include <osg/StateSet>
#include <osg/TextureRectangle>
#include <osg/TexMat>
#include <osg/ClipNode>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>
#include <osg/NodeVisitor>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/ShapeDrawable>
#include <osg/Transform>
#include <osg/Material>
#include <osg/NodeCallback>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/TextureCubeMap>

#include <osgDB//WriteFile>
#include <osgDB//ReadFile>

#include <osgUtil/Optimizer>

#include <iostream>

osg::ref_ptr<osg::Node> createClipNode(osg::ref_ptr<osg::Node> subgraph)
{
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//多边形线形绘制模式，正面和反面都绘制
	osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
	polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	//启用多边形线形绘制模式，并制定状态继承属性为OVERRIDE
	stateset->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	//多边形线形绘制节点
	osg::ref_ptr<osg::Group> wireframe_subgraph = new osg::Group;
	//设置渲染状态
	wireframe_subgraph->setStateSet(stateset);
	wireframe_subgraph->addChild(subgraph);

	root->addChild(wireframe_subgraph);

	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
	//更新回调，实现动态裁剪
	osg::ref_ptr<osg::NodeCallback> nc = new osg::AnimationPathCallback(subgraph->getBound().center(), osg::Vec3(0.0f, 0.0f, 1.0f), osg::inDegrees(45.0f));
	transform->setUpdateCallback(nc);

	//创建裁剪节点
	osg::ref_ptr<osg::ClipNode> clipnode = new osg::ClipNode;
	osg::BoundingSphere bs = subgraph->getBound();
	bs.radius() *= 0.4f;
	//设置裁剪节点的包围盒
	osg::BoundingBox bb;
	bb.expandBy(bs);
	//根据前面指定的包围盒创建6个裁剪平面
	clipnode->createClipBox(bb);
	//禁用拣选
	clipnode->setCullingActive(false);
	transform->addChild(clipnode);
	root->addChild(transform);
	//创建未被裁剪的节点
	osg::ref_ptr<osg::Group> clippedNode = new osg::Group;
	clippedNode->setStateSet(clipnode->getStateSet());
	clippedNode->addChild(subgraph);

	root->addChild(clippedNode);
	return root;
}

//创建一个四边形节点
osg::ref_ptr<osg::Node> createNode()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//设置顶点
	osg::ref_ptr<osg::Vec3Array> vc = new osg::Vec3Array;
	vc->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	vc->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	vc->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

	geom->setVertexArray(vc);

	//设置纹理坐标
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 1.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));

	geom->setTexCoordArray(0, vt);

	//设置法线
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array;
	nc->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));

	geom->setNormalArray(nc);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	//添加图元
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	//绘制
	geode->addDrawable(geom);

	return geode;
}
//创建二维纹理状态对象
osg::ref_ptr<osg::StateSet> createTexture2DState(osg::ref_ptr<osg::Image> image)
{
	//创建状态对象
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//创建二维纹理对象
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::DYNAMIC);
	//设置贴图
	texture->setImage(image);
	stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	return stateset;
}
//创建一个四边形
osg::ref_ptr<osg::Geode> createQuard()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geode->addChild(geom);
	//设置顶点
	osg::ref_ptr<osg::Vec3Array> vec = new osg::Vec3Array;
	vec->push_back(osg::Vec3(-10.0f, 0.0f, -10.0f));
	vec->push_back(osg::Vec3(-10.0f, 0.0f, 10.0f));
	vec->push_back(osg::Vec3(10.0f, 0.0f, 10.0f));
	vec->push_back(osg::Vec3(10.0f, 0.0f, -10.0f));
	geom->setVertexArray(vec);
	//设置法线
	osg::ref_ptr<osg::Vec3Array> nor = new osg::Vec3Array;
	nor->push_back(osg::Vec3f(0.0f, -1.0f, 0.0f));
	geom->setNormalArray(nor);
	geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
	//设置纹理坐标
	osg::ref_ptr<osg::Vec2Array> tex = new osg::Vec2Array;
	tex->push_back(osg::Vec2f(0.0f, 0.0f));
	tex->push_back(osg::Vec2f(0.0f, 1.0f));
	tex->push_back(osg::Vec2f(1.0f, 1.0f));
	tex->push_back(osg::Vec2f(1.0f, 0.0f));
	geom->setTexCoordArray(0, tex);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	return geode;
}
static void fillImage(unsigned char* ptr, unsigned int size)
{
	//白色
	if (size == 1)
	{
		float r = 0.5f;
		osg::Vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
		*ptr++ = (unsigned char)(color[0] * 255.0f);
		*ptr++ = (unsigned char)(color[1] * 255.0f);
		*ptr++ = (unsigned char)(color[2] * 255.0f);
		*ptr++ = (unsigned char)(color[3] * 255.0f);
	}
	//蓝色
	if (size == 2)
	{
		osg::Vec4 color(0.0f, 0.0f, 1.0f, 1.0f);
		for (unsigned int r = 0; r < size; ++r)
		{
			for (unsigned int c = 0; c < size; ++c)
			{
				*ptr++ = (unsigned char)(color[0] * 255.0f);
				*ptr++ = (unsigned char)(color[1] * 255.0f);
				*ptr++ = (unsigned char)(color[2] * 255.0f);
				*ptr++ = (unsigned char)(color[3] * 255.0f);
			}
		}
	}
	//黄色
	if (size == 4)
	{
		osg::Vec4 color(0.0f, 1.0f, 0.0f, 1.0f);
		for (unsigned int r = 0; r < size; ++r)
		{
			for (unsigned int c = 0; c < size; ++c)
			{
				*ptr++ = (unsigned char)(color[0] * 255.0f);
				*ptr++ = (unsigned char)(color[1] * 255.0f);
				*ptr++ = (unsigned char)(color[2] * 255.0f);
				*ptr++ = (unsigned char)(color[3] * 255.0f);
			}
		}
	}
	//红色
	if (size == 8)
	{
		osg::Vec4 color(1.0f, 0.0f, 0.0f, 1.0f);
		for (unsigned int r = 0; r < size; ++r)
		{
			for (unsigned int c = 0; c < size; ++c)
			{
				*ptr++ = (unsigned char)(color[0] * 255.0f);
				*ptr++ = (unsigned char)(color[1] * 255.0f);
				*ptr++ = (unsigned char)(color[2] * 255.0f);
				*ptr++ = (unsigned char)(color[3] * 255.0f);
			}
		}
	}
	//粉红色
	if (size == 16)
	{
		osg::Vec4 color(1.0f, 0.0f, 1.0f, 1.0f);
		for (unsigned int r = 0; r < size; ++r)
		{
			for (unsigned int c = 0; c < size; ++c)
			{
				*ptr++ = (unsigned char)(color[0] * 255.0f);
				*ptr++ = (unsigned char)(color[1] * 255.0f);
				*ptr++ = (unsigned char)(color[2] * 255.0f);
				*ptr++ = (unsigned char)(color[3] * 255.0f);
			}
		}
	}
	//黄色
	if (size == 64)
	{
		osg::Vec4 color(0.0f, 1.0f, 1.0f, 1.0f);
		for (unsigned int r = 0; r < size; ++r)
		{
			for (unsigned int c = 0; c < size; ++c)
			{
				*ptr++ = (unsigned char)(color[0] * 255.0f);
				*ptr++ = (unsigned char)(color[1] * 255.0f);
				*ptr++ = (unsigned char)(color[2] * 255.0f);
				*ptr++ = (unsigned char)(color[3] * 255.0f);
			}
		}
	}
	//灰白色
	if (size == 18)
	{
		osg::Vec4 color(0.5f, 0.5f, 0.5f, 1.0f);
		for (unsigned int r = 0; r < size; ++r)
		{
			for (unsigned int c = 0; c < size; ++c)
			{
				*ptr++ = (unsigned char)(color[0] * 255.0f);
				*ptr++ = (unsigned char)(color[1] * 255.0f);
				*ptr++ = (unsigned char)(color[2] * 255.0f);
				*ptr++ = (unsigned char)(color[3] * 255.0f);
			}
		}
	}
	//黑色
	if (size == 256)
	{
		osg::Vec4 color(0.0f, 0.0f, 0.0f, 1.0f);
		for (unsigned int r = 0; r < size; ++r)
		{
			for (unsigned int c = 0; c < size; ++c)
			{
				*ptr++ = (unsigned char)(color[0] * 255.0f);
				*ptr++ = (unsigned char)(color[1] * 255.0f);
				*ptr++ = (unsigned char)(color[2] * 255.0f);
				*ptr++ = (unsigned char)(color[3] * 255.0f);
			}
		}
	}
}
//创建二维纹理状态对象
osg::ref_ptr<osg::StateSet> createTexture2DState1(osg::ref_ptr<osg::Image> image)
{
	//创建状态集对象
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//创建二维纹理对象
	osg::ref_ptr<osg::TextureRectangle> texture = new osg::TextureRectangle;
	texture->setDataVariance(osg::Object::DYNAMIC);
	//设置贴图
	texture->setImage(image);
	//设置纹理矩阵，并设置为根据矩阵纹理（TextureRectangle）的大小自动缩放
	//从而允许应用一个矩形纹理到一个纹理坐标不在0-1上
	osg::ref_ptr<osg::TexMat> texmat=new osg::TexMat;
	texmat->setScaleByTextureRectangleSize(true);
	//启用纹理及纹理矩阵
	stateset->setTextureAttributeAndModes(0,texmat,osg::StateAttribute::ON);
	stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	//关闭光照
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return stateset;
}
//创建二维纹理属性
osg::ref_ptr<osg::StateSet> createTexture1DState()
{
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	//创建二维纹理
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	//设置边界处理
	texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	//设置滤波
	texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::NEAREST);
	//设置贴图
	texture->setImage(image);
	//设置自动纹理坐标，并指定相关的平面
	osg::ref_ptr<osg::TexGen> texgen=new osg::TexGen;
	texgen->setMode(osg::TexGen::OBJECT_LINEAR);
	texgen->setPlane(osg::TexGen::S,osg::Plane(0.0f,0.0f,1.0f,0.0f));
	//创建属性集
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//启用二维纹理
	stateset->setTextureAttribute(0, texture, osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//启用纹理坐标生成器
	stateset->setTextureAttribute(0, texgen, osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_GEN_T, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	return stateset;
}
//纹理坐标生成器，继承自NodeVisitor
class TexCoordGenerator:public osg::NodeVisitor
{
public:
	//遍历所有的子节点
	TexCoordGenerator() :NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		//
	}
	void apply(osg::Geode& geode)
	{
		//通过包围盒来确定合适的比例
		const osg::BoundingSphere &bsphere = geode.getBound();
		float scale = 10;
		if (bsphere.radius()!=0)
		{
			scale = 5 / bsphere.radius();
		}
		//遍历所有几何体，并设置纹理坐标
		for (unsigned i = 0; i < geode.getNumDrawables();++i)
		{
			osg::Geometry* geo = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
			if (geo)
			{
				osg::Vec2Array* tc = generate_coords(geo->getVertexArray(), geo->getNormalArray(), scale);
				geo->setTexCoordArray(0, tc);
			}
		}
		NodeVisitor::apply(geode);
	}
protected:
	//计算纹理坐标
	osg::Vec2Array* generate_coords(osg::Array* vx, osg::Array* nx, float scale)
	{
		osg::Vec2Array* v2a = dynamic_cast<osg::Vec2Array*>(vx);
		osg::Vec3Array* v3a = dynamic_cast<osg::Vec3Array*>(vx);
		osg::Vec4Array* v4a = dynamic_cast<osg::Vec4Array*>(vx);
		osg::Vec2Array* n2a = dynamic_cast<osg::Vec2Array*>(nx);
		osg::Vec3Array* n3a = dynamic_cast<osg::Vec3Array*>(nx);
		osg::Vec4Array* n4a = dynamic_cast<osg::Vec4Array*>(nx);

		osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array;
		for (unsigned i = 0; i < vx->getNumElements();++i)
		{
			osg::Vec3 P;
			if (v2a) P.set((*v2a)[i].x(),(*v2a)[i].y(),0);
			if (v3a) P.set((*v3a)[i].x(), (*v3a)[i].y(), (*v3a)[i].z());
			if (v4a) P.set((*v4a)[i].x(), (*v4a)[i].y(), (*v4a)[i].z());
			osg::Vec3 N(0, 0, 1);
			if (n2a) N.set((*n2a)[i].x(), (*n2a)[i].y(), 0);
			if (n3a) N.set((*n3a)[i].x(), (*n3a)[i].y(), (*n3a)[i].z());
			if (n4a) N.set((*n4a)[i].x(), (*n4a)[i].y(), (*n4a)[i].z());
			int axis = 0;
			if (N.y() > N.x() && N.y() > N.z())axis = 1;
			if (-N.y() > N.x() && -N.y() > N.z())axis = 1;
			if (N.z() > N.x() && N.z() > N.y())axis = 2;
			if (-N.z() > N.x() && -N.z() > N.y())axis = 2;
			osg::Vec2 uv;
			switch (axis)
			{
			case 0: uv.set(P.y(), P.z()); break;
			case 1: uv.set(P.x(), P.z()); break;
			case 2: uv.set(P.x(), P.y()); break;
			default:;
			}
			tc->push_back(uv*scale);
		}
		return tc.release();
	}
};
//创建二维纹理状态对象
osg::ref_ptr<osg::StateSet> createTexture2DState2(osg::ref_ptr<osg::Image> image)
{
	//创建状态集对象
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//创建二维纹理对象
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::DYNAMIC);
	//设置贴图
	texture->setImage(image);
	texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
	stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	return stateset;
}
//读取立方图
osg::ref_ptr<osg::TextureCubeMap> readCubeMap()
{
	osg::ref_ptr<osg::TextureCubeMap> cubemap = new osg::TextureCubeMap;
	osg::ref_ptr<osg::Image> imagePosX = osgDB::readImageFile("right.jpg");
	osg::ref_ptr<osg::Image> imageNegX = osgDB::readImageFile("left.jpg");
	osg::ref_ptr<osg::Image> imagePosY = osgDB::readImageFile("front.jpg");
	osg::ref_ptr<osg::Image> imageNegY = osgDB::readImageFile("back.jpg");
	osg::ref_ptr<osg::Image> imagePosZ = osgDB::readImageFile("up.jpg");
	osg::ref_ptr<osg::Image> imageNegZ = osgDB::readImageFile("down.jpg");
	if (imagePosX.get()&&imageNegX.get()&&imagePosY.get()&&imageNegY.get()&&imagePosZ.get()&&imageNegZ.get())
	{
		//设置立方图的6个面的贴图
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX.get());
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX.get());
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY.get());
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY.get());
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ.get());
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ.get());
		//设置纹理环绕模式
		cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
		//设置滤波：线形和mipmap
		cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	}
	return cubemap.get();
}
//更新立方体图纹理
struct TexMatCallback:public osg::NodeCallback 
{
public:
	//纹理矩阵
	osg::TexMat& _texMat;
	TexMatCallback(osg::TexMat& tm) :_texMat(tm)
	{
		//
	}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			//得到模型视图矩阵并设置旋转角度
			const osg::Matrix& MV = *(cv->getModelViewMatrix());
			const osg::Matrix R = osg::Matrix::rotate(osg::DegreesToRadians(112.0f), 0.0f, 0.0f, 1.0f)*
				osg::Matrix::rotate(osg::DegreesToRadians(90.0f), 1.0f, 0.0f, 0.0f);
			osg::Quat q = MV.getRotate();
			const osg::Matrix C = osg::Matrix::rotate(q.inverse());
			//设置纹理矩阵
			_texMat.setMatrix(C*R);
		}
	}
};
//一个变换类，使天空盒绕视点旋转
class MoveEarthSkyWithPointTranform:public osg::Transform
{
public:
	//局部矩阵计算成世界矩阵
	virtual bool computerLocalToWorldMatrix(osg::Matrix&matrix, osg::NodeVisitor* nv)const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMult(osg::Matrix::translate(eyePointLocal));
		}
		return true;
	}
	//世界矩阵计算为局部矩阵
	virtual bool computerWorldToLocalMatrix(osg::Matrix&matrix, osg::NodeVisitor* nv)const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMult(osg::Matrix::translate(-eyePointLocal));
		}
		return true;
	}
};
//创建天空盒
osg::ref_ptr<osg::Node> createSkyBox()
{
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//设置纹理映射方式，指定为替代方式，即纹理中的颜色代替原来的颜色
	osg::ref_ptr<osg::TexEnv> te = new osg::TexEnv;
	te->setMode(osg::TexEnv::REPLACE);
	stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//自动生成纹理坐标，反射方式(REFLECTION_MAP)
	/*
	NORMAL_MAP 标准模式-立方图纹理
	REFLECTION_MAP 反射模式-球体纹理
	SPHERE_MAP 球体模型-球体纹理
	*/
	osg::ref_ptr<osg::TexGen> tg=new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);
	stateset->setTextureAttributeAndModes(0,tg,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	//设置纹理矩阵
	osg::ref_ptr<osg::TexMat> tm = new osg::TexMat;
	stateset->setTextureAttribute(0, tm);
	//设置立方图纹理
	osg::ref_ptr<osg::TextureCubeMap> skymap = readCubeMap();
	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
	//将深度设置为远平面
	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0, 1.0);//远平面
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//设置渲染顺序为-1，先渲染
	stateset->setRenderBinDetails(-1, "RenderBin");
	osg::ref_ptr<osg::Drawable> drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 1));
	//把球体加入到叶节点
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setCullingActive(false);
	geode->setStateSet(stateset);
	geode->addDrawable(drawable);
	//设置变换
	osg::ref_ptr<osg::Transform> tranform = new MoveEarthSkyWithPointTranform();
	tranform->setCullingActive(false);
	tranform->addChild(geode);
	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setCullCallback(new TexMatCallback(*tm));
	clearNode->addChild(tranform);
	return clearNode;
}


int main()
{
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Node> root = new osg::Node;
	//加载模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");
	root = createClipNode(node);

	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //渲染状态
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读取贴图文件
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	osg::ref_ptr<osg::Node> node = createNode();
	//创建状态集对象
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture2DState(image);
	//使用二维纹理
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
#endif //二维纹理映射
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//读取贴图
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	if (image)
	{
		//创建二维纹理
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		texture->setImage(image);
		//设置自动生成坐标
		osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen;
		texgen->setMode(osg::TexGen::SPHERE_MAP);
		//设置纹理环境，模式为BLEND
		osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
		texenv->setMode(osg::TexEnv::BLEND);
		//设置BLEND操作的颜色
		texenv->setColor(osg::Vec4(0.6f, 0.6f, 0.6f, 0.0f));
		//启动单元1自动生成纹理坐标，并使用纹理
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		stateset->setTextureAttributeAndModes(1, texture, osg::StateAttribute::ON);
		stateset->setTextureAttributeAndModes(1, texgen, osg::StateAttribute::ON);
		//设置纹理环境
		stateset->setTextureAttribute(1, texenv);
		//设置纹理状态
		node->setStateSet(stateset);
	}
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif // 多重纹理映射
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//创建一个平面
	osg::ref_ptr<osg::Geode> geode = createQuard();
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	osg::ref_ptr<osg::Image> image = new osg::Image;
	//创建一个MipmapDataType列表，用来存放各层图片数据的偏移地址
	osg::Image::MipmapDataType mipmapData;
	//纹理的尺寸的最大值，必须为2的幂次
	unsigned int s = 256;
	//计算所需分配的数组的大小
	unsigned int totalSize = 0;
	for (unsigned int i = 0; s > 0; s >>= 1, ++i)
	{
		if (i > 0)
		{
			mipmapData.push_back(totalSize);
		}
		totalSize += s*s * 4;
	}
	//申请一个数据
	unsigned char* ptr = new unsigned char[totalSize];
	//设置image的尺寸大小、数据及数据格式
	image->setImage(256, 256, 256, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, ptr, osg::Image::USE_NEW_DELETE, 1);
	//将偏移地址传入image对象
	image->setMipmapLevels(mipmapData);
	//向image中填充各层数据
	s = 256;
	for (unsigned int i = 0; s > 0; s >>= 1, ++i)
	{
		fillImage(ptr, s);
		ptr += s*s * 4;
	}
	//创建一个二维纹理对象
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	//设置贴图
	texture->setImage(0, image);
	//设置边界处理为REPEATE
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	//设置滤波
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST_MIPMAP_NEAREST);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
	//启用二维纹理对象
	stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	geode->setStateSet(stateset);
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //MipMap纹理映射
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读取贴图文件
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	osg::ref_ptr<osg::Node> node = createNode();
	//创建状态集对象
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture2DState1(image);
	//使用二维纹理
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //TextureRectangle纹理映射
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");
	//自动生成纹理坐标属性
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture1DState();
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif // 自动生成纹理坐标
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("dumptruck.osg");
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	//计算纹理坐标
	TexCoordGenerator tcg;
	node->accept(tcg);
	//创建状态集对象
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture2DState2(image);
	//使用二维纹理
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //计算纹理坐标
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> rootnode = new osg::Group;
	//加入天空盒
	rootnode->addChild(createSkyBox());
	viewer->setSceneData(rootnode);
	viewer->realize();
	viewer->run();
	return 0;
#endif //立方图纹理


}