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
#include <osg/Stencil>
#include <osg/ColorMask>
#include <osg/GLExtensions>
#include <osg/Depth>
#include <osg/Texture3D>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/BoundingSphere>
#include <osg/BoundingBox>
#include <osg/TexGenNode>
#include <osg/Material>

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
//定义相机绘制回调
struct  MyCameraPostDrawCallBack :public osg::Camera::DrawCallback
{
public:
	MyCameraPostDrawCallBack(osg::ref_ptr<osg::Image> image):
		_image(image)
	{

	}
	virtual void operator()(const osg::Camera&/*camera*/) const
	{
		if (_image&&_image->getPixelFormat()==GL_RGBA&&_image->getDataType()==GL_UNSIGNED_BYTE)
		{
			//获得Image的中心
			int column_start = _image->s() / 4;
			int column_end = 3 * column_start;

			int row_start = _image->t() / 4;
			int row_end = 3 * row_start;
			//将像素数据进行反向
			for (int r = row_start; r < row_end;++r)
			{
				unsigned char* data = _image->data(column_start, r);
				for (int c = column_start; c < column_end;++c)
				{
					(*data) = 255 - (*data); ++data;
					(*data) = 255 - (*data); ++data;
					(*data) = 255 - (*data); ++data;
					(*data) = 255; ++data;
				}
			}
			_image->dirty();
		}
		else if (_image&&_image->getPixelFormat()==GL_RGBA&&_image->getDataType()==GL_FLOAT)
		{
			//获得Image的中心
			int column_start = _image->s() / 4;
			int column_end = 3 * column_start;

			int row_start = _image->t() / 4;
			int row_end = 3 * row_start;

			//将像素数据进行反向
			for (int r = row_start; r < row_end;++r)
			{
				float* data = (float*)_image->data(column_start, r);
				for (int c = column_start; c < column_end;++c)
				{
					(*data) = 1.0f - (*data); ++data;
					(*data) = 1.0f - (*data); ++data;
					(*data) = 1.0f - (*data); ++data;
					(*data) = 1.0f; ++data;
				}
			}
			_image->dirty();
		}
	}
public:
	osg::ref_ptr<osg::Image> _image;
};
//创建预渲染场景
osg::ref_ptr<osg::Node> createPreRenderSubGraph(osg::ref_ptr<osg::Node> subgraph,
	unsigned tex_width,
	unsigned tex_height,
	osg::Camera::RenderTargetImplementation renderImplementation,
	bool useImage)
{
	if (!subgraph) return 0;
	//创建一个包含预渲染camera的Group节点
	osg::ref_ptr<osg::Group> parent = new osg::Group;
	//创建纹理，用来绑定相机渲染的结果
	osg::ref_ptr<osg::Texture> texture = 0;
	{
		osg::ref_ptr<osg::Texture2D> texture2D = new osg::Texture2D;
		texture2D->setTextureSize(tex_width, tex_height);
		texture2D->setInternalFormat(GL_RGBA);
		texture2D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		texture2D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		texture = texture2D;
	}
	//创建一个用来浏览的四边形几何体
	{
		osg::ref_ptr<osg::Geometry> polyGeom = new osg::Geometry;
		//设置该几何体不适用显示列表
		polyGeom->setSupportsDisplayList(false);
		float height = 100.0f;
		float width = 200.0f;
		//创建顶点数组，并添加数据
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
		vertices->push_back(osg::Vec3(width, 0.0f, 0.0f));
		vertices->push_back(osg::Vec3(width, 0.0f, height));
		vertices->push_back(osg::Vec3(0.0f, 0.0f, height));
		//创建纹理数组，并添加数据
		osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
		texcoords->push_back(osg::Vec2(0.0f, 0.0f));
		texcoords->push_back(osg::Vec2(1.0f, 0.0f));
		texcoords->push_back(osg::Vec2(1.0f, 1.0f));
		texcoords->push_back(osg::Vec2(0.0f, 1.0f));
		polyGeom->setVertexArray(vertices);
		//使用vbo扩展
		{
			osg::ref_ptr<osg::VertexBufferObject> vbObject = new osg::VertexBufferObject;
			vertices->setVertexBufferObject(vbObject);
			polyGeom->setUseVertexBufferObjects(true);
		}
		polyGeom->setTexCoordArray(0, texcoords);
		//创建颜色数组，并设置绑定方式及添加数据
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		polyGeom->setColorArray(colors);
		polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
		polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vertices->size()));
		//现在需要将纹理附加到该几何体上，创建一个包含该纹理的Stateset
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
		polyGeom->setStateSet(stateset);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(polyGeom);
		parent->addChild(geode);
	}
	//需要创建一个相机节点，用来渲染到该纹理（RTT）
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		//设置背景色并清除颜色和深度缓存
		camera->setClearColor(osg::Vec4(0.1f, 0.1f, 0.3f, 1.0f));
		camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//获得该节点的范围盒
		const osg::BoundingSphere& bs = subgraph->getBound();
		if (!bs.valid())
		{
			return subgraph;
		}
		float znear = 1.0f*bs.radius();
		float zfar = 3.0f*bs.radius();
		float proj_top = 0.25f*znear;
		float proj_right = 0.5f*znear;
		znear *= 0.9f;
		zfar *= 1.1f;
		//设置投影矩阵
		camera->setProjectionMatrixAsFrustum(-proj_right, proj_right, -proj_top, proj_top, znear, zfar);
		//将相机对准该子场景
		camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		camera->setViewMatrixAsLookAt(bs.center() - osg::Vec3(0.0f, 2.0f, 0.0f)*bs.radius(), bs.center(), osg::Vec3(0.0f, 0.0f, 1.0f));
		//设置视口
		camera->setViewport(0, 0, tex_width, tex_height);
		//设置相机的渲染序列
		camera->setRenderOrder(osg::Camera::PRE_RENDER);
		//设置相机渲染，通过OpenGL frame buffer object 实现
		camera->setRenderTargetImplementation(renderImplementation);
		if (useImage)
		{
			osg::ref_ptr<osg::Image> image = new osg::Image;
			//image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
			image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_FLOAT);
			//将Image附加到相机的COLOR_BUFFER
			camera->attach(osg::Camera::COLOR_BUFFER, image);
			//添加相机的绘制后回调，修改images数据
			camera->setPostDrawCallback(new MyCameraPostDrawCallBack(image));
			//这里不直接将相机的COLOR_BUFFER附加到该纹理上是为了修改渲染后的图像数据
			texture->setImage(0, image);
		}
		else
		{
			//直接将该纹理附加到相机的颜色缓存
			camera->attach(osg::Camera::COLOR_BUFFER, texture);
		}
		//添加要绘制的子场景
		camera->addChild(subgraph);
		parent->addChild(camera);
	}
	return parent;
}
//创建一维纹理属性
osg::ref_ptr<osg::StateSet> createTexture1DState1()
{
	//创建贴图对象，实际上是一个高度为1的二维图像
	osg::ref_ptr<osg::Image> image = new osg::Image;
	//为Image分配一个空间
	image->allocateImage(1024, 1, 1, GL_RGBA, GL_FLOAT);
	//设置纹理图像数据格式RGBA
	image->setInternalTextureFormat(GL_RGBA);
	//为Image填充数据
	osg::Vec4* dataPtr = (osg::Vec4*)image->data();
	for (int i = 0; i < 1024;++i)
	{
		*dataPtr++ = osg::Vec4(1.0f, 0.5f, 0.8f, 0.5f);
	}
	//创建一维纹理
	osg::ref_ptr<osg::Texture1D> texture = new osg::Texture1D;
	//设置环绕模式
	texture->setWrap(osg::Texture1D::WRAP_S, osg::Texture1D::MIRROR);
	//设置滤波
	texture->setFilter(osg::Texture1D::MIN_FILTER, osg::Texture1D::LINEAR);
	//设置贴图
	texture->setImage(image);
	//设置自动纹理坐标，并指定相关的平面
	osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen;
	texgen->setMode(osg::TexGen::OBJECT_LINEAR);
	texgen->setPlane(osg::TexGen::S, osg::Plane(0.0f, 0.0f, 1.0f, -10000));
	//创建属性集
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//启用一维纹理
	stateset->setTextureAttribute(0, texture, osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_1D, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//启用纹理生成器
	stateset->setTextureAttribute(0, texgen, osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	return stateset;
}
//初始化一个图形环境
class MyGraphicsContext
{
public:
	MyGraphicsContext()
	{
		//设置图形环境特性
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		//设置左上角坐标
		traits->x = 0;
		traits->y = 0;
		//设置宽度和高度
		traits->width = 1;
		traits->height = 1;
		//设置双缓冲
		traits->doubleBuffer = false;
		traits->sharedContext = 0;
		//设置pbuffer
		traits->pbuffer = true;
		//创建图形环境
		_gc = osg::GraphicsContext::createGraphicsContext(traits);
		//如果创建失败
		if (!_gc)
		{
			//设置pbuffer为false
			traits->pbuffer = false;
			//重新创建图形环境
			_gc = osg::GraphicsContext::createGraphicsContext(traits);
		}
		//是否初始化
		if (!_gc.valid())
		{
			//初始化
			_gc->realize();
			_gc->makeCurrent();
		}
	}
	bool valid() const{ return _gc.valid() && _gc->isRealized(); }
protected:
private:
	osg::ref_ptr<osg::GraphicsContext> _gc;
};
//向场景中添加光源
osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Group> lightRoot = new osg::Group;
	lightRoot->addChild(node);
	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);
	//计算包围盒
	osg::BoundingSphere bs;
	node->computeBound();
	bs = node->getBound();
	//创建一个Light对象
	osg::ref_ptr<osg::Light> light = new osg::Light;
	light->setLightNum(0);
	//设置方向
	light->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	//设置位置
	light->setPosition(osg::Vec4(bs.center().x(), bs.center().y(), bs.center().z() + bs.radius(), 1.0f));
	//设置环境光的颜色
	light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//设置散射光的颜色
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//设置恒衰减指数
	light->setConstantAttenuation(1.0f);
	//设置线性衰减指数
	light->setLinearAttenuation(0.0f);
	//设置二次方衰减指数
	light->setQuadraticAttenuation(0.0f);
	//创建光源
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource;
	lightSource->setLight(light);
	lightRoot->addChild(lightSource);
	return lightRoot;
}
//创建聚光灯纹理的mipmap贴图
osg::ref_ptr<osg::Image> createSpotLightImage(const osg::Vec4& centerColor, const osg::Vec4& backgroudColour, unsigned int size, float power)
{
	//创建Image对象
	osg::ref_ptr<osg::Image> image = new osg::Image;
	//动态分配一个size*size大小的image
	image->allocateImage(size, size, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	//填充image以中心为原点，颜色逐渐向四周衰减
	float mid = (float(size) - 1)*0.5f;
	float div = 2.0f / float(size);
	for (unsigned int r = 0; r < size;++r)
	{
		unsigned char* ptr = image->data(0, r, 0);
		for (unsigned int c = 0; c < size;++c)
		{
			float dx = (float(c) - mid)*div;
			float dy = (float(r) - mid)*div;
			float r = powf(1.0f - sqrtf(dx*dx + dy*dy), power);
			if (r < 0.0f) r = 0.0f;
			osg::Vec4 color = centerColor*r + backgroudColour*(1.0f - r);
			*ptr++ = (unsigned char)((color[0])*255.0f);
			*ptr++ = (unsigned char)((color[1])*255.0f);
			*ptr++ = (unsigned char)((color[2])*255.0f);
			*ptr++ = (unsigned char)((color[3])*255.0f);
		}
	}
	return image;
}
//创建聚光灯状态属性
osg::ref_ptr<osg::StateSet> createSpotLightDecoratorState(unsigned int lightNum, unsigned int textureUnit)
{
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//开启ID为lightNum的光照
	stateset->setMode(GL_LIGHT0 + lightNum, osg::StateAttribute::ON);
	//设置中心的颜色和环境光的颜色
	osg::Vec4 centerColour(1.0f, 1.0f,1.0f, 1.0f);
	osg::Vec4 ambientColour(0.05f, 0.05f, 0.05f, 1.0f);
	//创建聚光灯纹理
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(createSpotLightImage(centerColour, ambientColour, 64, 1.0));
	texture->setBorderColor(osg::Vec4(ambientColour));
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
	texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER);
	//打开纹理单元
	stateset->setTextureAttributeAndModes(textureUnit, texture, osg::StateAttribute::ON);
	//设置自动生成纹理坐标
	stateset->setTextureMode(textureUnit, GL_TEXTURE_GEN_S, osg::StateAttribute::ON);
	stateset->setTextureMode(textureUnit, GL_TEXTURE_GEN_T, osg::StateAttribute::ON);
	stateset->setTextureMode(textureUnit, GL_TEXTURE_GEN_R, osg::StateAttribute::ON);
	stateset->setTextureMode(textureUnit, GL_TEXTURE_GEN_Q, osg::StateAttribute::ON);
	return stateset;
}
//创建聚光灯节点
osg::ref_ptr<osg::Node> createSpotLightNode(const osg::Vec3& position, const osg::Vec3& direction, float angle, unsigned int lightNum, unsigned int textureUnit)
{
	osg::ref_ptr<osg::Group> group = new osg::Group;
	//创建光源
	osg::ref_ptr<osg::LightSource> lightsource = new osg::LightSource;
	osg::ref_ptr<osg::Light> light = lightsource->getLight();
	light->setLightNum(lightNum);
	light->setPosition(osg::Vec4(position,1.0f));
	light->setAmbient(osg::Vec4(0.00f, 0.00f, 0.05f, 1.0f));
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	group->addChild(lightsource);
	//计算法向量
	osg::Vec3 up(0.0f, 0.0f, 1.0f);
	up = (direction^up) ^ direction;
	up.normalize();
	//创建自动生成纹理坐标节点
	osg::ref_ptr<osg::TexGenNode> texgenNode = new osg::TexGenNode;
	//关联纹理单元
	texgenNode->setTextureUnit(textureUnit);
	//设置纹理坐标生成器
	osg::ref_ptr<osg::TexGen> texgen = texgenNode->getTexGen();
	//设置模式为视觉线性
	texgen->setMode(osg::TexGen::EYE_LINEAR);
	//从视图中指定参考平面
	texgen->setPlanesFromMatrix(osg::Matrixd::lookAt(position, position + direction, up)* osg::Matrixd::perspective(angle, 1.0, 0.1, 100));
	group->addChild(texgenNode);
	return group;
}
//创建动画路径
osg::ref_ptr<osg::AnimationPath> createAnimationPath(const osg::Vec3& center, float radius, double looptime)
{
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::LOOP);
	int numSamples = 40;
	float yaw = 0.0f;
	float yaw_delta = 2.0f*osg::PI / ((float)numSamples - 1.0f);
	float roll = osg::inDegrees(30.0f);
	double time = 0.0f;
	double time_delta = looptime / (double)numSamples;
	for (int i = 0; i < numSamples;++i)
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
//osg::ref_ptr<osg::Node> createBase(const osg::Vec3& center, float radius)
//{
//	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
//	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
//	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/lz.rgb");
//	if (image)
//	{
//		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
//		texture->setImage(image);
//		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
//	}
//	geode->setStateSet(stateset);
//	osg::ref_ptr<osg::HeightField> grid = new osg::HeightField;
//	grid->allocate(38, 39);
//	grid->setOrigin(center + osg::Vec3(-radius, -radius, 0.0f));
//	grid->setXInterval(radius*2.0f / (float)(38 - 1));
//	grid->setYInterval(radius*2.0f / (float)(39 - 1));
//	float minHeight = FLT_MAX;
//	float maxHeight = -FLT_MAX;
//	unsigned int r;
//	for (r = 0; r < 39;++r)
//	{
//		for (unsigned int c = 0; c < 38;++c)
//		{
//			float h = vertex[r + c * 39][2];
//			if (h>maxHeight) maxHeight = h;
//			if (h<minHeight) minHeight = h;
//		}
//	}
//	float heightScale = radius*0.5f / (maxHeight - minHeight);
//	float heightOffset = -(minHeight + maxHeight)*0.5f;
//	for (r = 0; r < 39;++r)
//	{
//		for (unsigned int c = 0; c < 38;++c)
//		{
//			float h = vertex[r + c * 39][2];
//			grid->setHeight(c, r, (h + heightOffset)*heightScale);
//		}
//	}
//	geode->addDrawable(new osg::ShapeDrawable(grid));
//	osg::ref_ptr<osg::Group> group = new osg::Group;
//	group->addChild(geode);
//	return group;
//}
//创建动画模型
osg::ref_ptr<osg::Node> createMovingModel(const osg::Vec3&center, float radius)
{
	float animationLength = 10.0f;
	osg::ref_ptr<osg::AnimationPath> animationPath = createAnimationPath(center, radius, animationLength);
	osg::ref_ptr<osg::Group> model = new osg::Group;
	osg::ref_ptr<osg::Node> cessns = osgDB::readNodeFile("cessna.osg");
	if (cessns)
	{
		const osg::BoundingSphere& bs = cessns->getBound();
		float size = radius / bs.radius()*0.3f;
		osg::ref_ptr<osg::MatrixTransform> positioned = new osg::MatrixTransform;
		positioned->setDataVariance(osg::Object::STATIC);
		positioned->setMatrix(osg::Matrix::translate(-bs.center())*
			osg::Matrix::scale(size, size, size)*
			osg::Matrix::rotate(osg::inDegrees(180.0f), 0.0f, 0.0f,2.0f));
		positioned->addChild(cessns);

		osg::ref_ptr<osg::MatrixTransform> xform = new osg::MatrixTransform;
		xform->setUpdateCallback(new osg::AnimationPathCallback(animationPath, 0.0f, 2.0));
		xform->addChild(positioned);
		//添加聚光顶节点
		xform->addChild(createSpotLightNode(osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 1.0f, -1.0f), 60.0f, 0, 1));
		model->addChild(xform);
	}
	return model;
}
//创建场景
//osg::ref_ptr<osg::Node> createModel()
//{
//	osg::Vec3 center(0.0f, 0.0f, 0.0f);
//	float radius = 100.0f;
//	//创建动画模型
//	osg::ref_ptr<osg::Node> shadower = createMovingModel(center, radius*0.5f);
//	//创建地形平面
//	osg::ref_ptr<osg::Node> shadowed = createBase(center - osg::Vec3(0.0f, 0.0f, radius*0.1), radius);
//	//创建场景组节点
//	osg::ref_ptr<osg::Group> root = new osg::Group;
//	//设置状态属性
//	root->setStateSet(createSpotLightDecoratorState(0, 1));
//	//添加子节点
//	root->addChild(shadower);
//	root->addChild(shadowed);
//	return root;
//}
//



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
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	unsigned tex_width = 1024;
	unsigned tex_height = 512;
	osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
	bool useImage = false;
	//读取模型
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("cessna.osg");
	//创建一个transform节点，用来选装该子场景
	osg::ref_ptr<osg::MatrixTransform> loadedModelTransform = new osg::MatrixTransform;
	loadedModelTransform->addChild(loadedModel);
	//设置更新回调
	osg::ref_ptr<osg::NodeCallback> nc = new osg::AnimationPathCallback(loadedModelTransform->getBound().center(), osg::Vec3(0.0f, 0.0f, 1.0f), osg::inDegrees(45.0f));
	loadedModelTransform->setUpdateCallback(nc);
	osg::ref_ptr<osg::Group> rootNode = new osg::Group;
	rootNode->addChild(createPreRenderSubGraph(loadedModelTransform, tex_width, tex_height, renderImplementation, useImage));
	viewer->setSceneData(rootNode);
	viewer->run();
	return 0;
#endif //渲染到纹理
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读取模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");
	//使用一维纹理属性
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture1DState1();
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //一维纹理
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读取模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//向场景中添加光源
	root->addChild(createLight(node));
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //简单光源
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createModel());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //聚光灯
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//读取模型
	osg::ref_ptr<osg::Node> node = createNode();
	//得到状态属性
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = node->getOrCreateStateSet();
	//创建材质对象
	osg::ref_ptr<osg::Material> mat = new osg::Material;
	//设置正面散射颜色
	mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//设置正面镜面颜色
	mat->setSpecular(osg::Material::FRONT, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//设置正面指数
	mat->setShininess(osg::Material::FRONT, 90.0f);
	stateset->setAttribute(mat);
	//设置背面剔除
	osg::ref_ptr<osg::CullFace> cullface = new osg::CullFace(osg::CullFace::BACK);
	stateset->setAttribute(cullface);
	stateset->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //材质




}