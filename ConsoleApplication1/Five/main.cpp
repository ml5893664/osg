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
	//��������λ���ģʽ������ͷ��涼����
	osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
	polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	//���ö�������λ���ģʽ�����ƶ�״̬�̳�����ΪOVERRIDE
	stateset->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	//��������λ��ƽڵ�
	osg::ref_ptr<osg::Group> wireframe_subgraph = new osg::Group;
	//������Ⱦ״̬
	wireframe_subgraph->setStateSet(stateset);
	wireframe_subgraph->addChild(subgraph);

	root->addChild(wireframe_subgraph);

	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
	//���»ص���ʵ�ֶ�̬�ü�
	osg::ref_ptr<osg::NodeCallback> nc = new osg::AnimationPathCallback(subgraph->getBound().center(), osg::Vec3(0.0f, 0.0f, 1.0f), osg::inDegrees(45.0f));
	transform->setUpdateCallback(nc);

	//�����ü��ڵ�
	osg::ref_ptr<osg::ClipNode> clipnode = new osg::ClipNode;
	osg::BoundingSphere bs = subgraph->getBound();
	bs.radius() *= 0.4f;
	//���òü��ڵ�İ�Χ��
	osg::BoundingBox bb;
	bb.expandBy(bs);
	//����ǰ��ָ���İ�Χ�д���6���ü�ƽ��
	clipnode->createClipBox(bb);
	//���ü�ѡ
	clipnode->setCullingActive(false);
	transform->addChild(clipnode);
	root->addChild(transform);
	//����δ���ü��Ľڵ�
	osg::ref_ptr<osg::Group> clippedNode = new osg::Group;
	clippedNode->setStateSet(clipnode->getStateSet());
	clippedNode->addChild(subgraph);

	root->addChild(clippedNode);
	return root;
}

//����һ���ı��νڵ�
osg::ref_ptr<osg::Node> createNode()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//���ö���
	osg::ref_ptr<osg::Vec3Array> vc = new osg::Vec3Array;
	vc->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	vc->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	vc->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

	geom->setVertexArray(vc);

	//������������
	osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array;
	vt->push_back(osg::Vec2(0.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 0.0f));
	vt->push_back(osg::Vec2(1.0f, 1.0f));
	vt->push_back(osg::Vec2(0.0f, 1.0f));

	geom->setTexCoordArray(0, vt);

	//���÷���
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array;
	nc->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));

	geom->setNormalArray(nc);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	//���ͼԪ
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	//����
	geode->addDrawable(geom);

	return geode;
}
//������ά����״̬����
osg::ref_ptr<osg::StateSet> createTexture2DState(osg::ref_ptr<osg::Image> image)
{
	//����״̬����
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//������ά�������
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::DYNAMIC);
	//������ͼ
	texture->setImage(image);
	stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	return stateset;
}
//����һ���ı���
osg::ref_ptr<osg::Geode> createQuard()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geode->addChild(geom);
	//���ö���
	osg::ref_ptr<osg::Vec3Array> vec = new osg::Vec3Array;
	vec->push_back(osg::Vec3(-10.0f, 0.0f, -10.0f));
	vec->push_back(osg::Vec3(-10.0f, 0.0f, 10.0f));
	vec->push_back(osg::Vec3(10.0f, 0.0f, 10.0f));
	vec->push_back(osg::Vec3(10.0f, 0.0f, -10.0f));
	geom->setVertexArray(vec);
	//���÷���
	osg::ref_ptr<osg::Vec3Array> nor = new osg::Vec3Array;
	nor->push_back(osg::Vec3f(0.0f, -1.0f, 0.0f));
	geom->setNormalArray(nor);
	geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
	//������������
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
	//��ɫ
	if (size == 1)
	{
		float r = 0.5f;
		osg::Vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
		*ptr++ = (unsigned char)(color[0] * 255.0f);
		*ptr++ = (unsigned char)(color[1] * 255.0f);
		*ptr++ = (unsigned char)(color[2] * 255.0f);
		*ptr++ = (unsigned char)(color[3] * 255.0f);
	}
	//��ɫ
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
	//��ɫ
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
	//��ɫ
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
	//�ۺ�ɫ
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
	//��ɫ
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
	//�Ұ�ɫ
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
	//��ɫ
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
//������ά����״̬����
osg::ref_ptr<osg::StateSet> createTexture2DState1(osg::ref_ptr<osg::Image> image)
{
	//����״̬������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//������ά�������
	osg::ref_ptr<osg::TextureRectangle> texture = new osg::TextureRectangle;
	texture->setDataVariance(osg::Object::DYNAMIC);
	//������ͼ
	texture->setImage(image);
	//����������󣬲�����Ϊ���ݾ�������TextureRectangle���Ĵ�С�Զ�����
	//�Ӷ�����Ӧ��һ����������һ���������겻��0-1��
	osg::ref_ptr<osg::TexMat> texmat=new osg::TexMat;
	texmat->setScaleByTextureRectangleSize(true);
	//���������������
	stateset->setTextureAttributeAndModes(0,texmat,osg::StateAttribute::ON);
	stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	//�رչ���
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return stateset;
}
//������ά��������
osg::ref_ptr<osg::StateSet> createTexture1DState()
{
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	//������ά����
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	//���ñ߽紦��
	texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	//�����˲�
	texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::NEAREST);
	//������ͼ
	texture->setImage(image);
	//�����Զ��������꣬��ָ����ص�ƽ��
	osg::ref_ptr<osg::TexGen> texgen=new osg::TexGen;
	texgen->setMode(osg::TexGen::OBJECT_LINEAR);
	texgen->setPlane(osg::TexGen::S,osg::Plane(0.0f,0.0f,1.0f,0.0f));
	//�������Լ�
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//���ö�ά����
	stateset->setTextureAttribute(0, texture, osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//������������������
	stateset->setTextureAttribute(0, texgen, osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_GEN_T, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	return stateset;
}
//�����������������̳���NodeVisitor
class TexCoordGenerator:public osg::NodeVisitor
{
public:
	//�������е��ӽڵ�
	TexCoordGenerator() :NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		//
	}
	void apply(osg::Geode& geode)
	{
		//ͨ����Χ����ȷ�����ʵı���
		const osg::BoundingSphere &bsphere = geode.getBound();
		float scale = 10;
		if (bsphere.radius()!=0)
		{
			scale = 5 / bsphere.radius();
		}
		//�������м����壬��������������
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
	//������������
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
//������ά����״̬����
osg::ref_ptr<osg::StateSet> createTexture2DState2(osg::ref_ptr<osg::Image> image)
{
	//����״̬������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//������ά�������
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::DYNAMIC);
	//������ͼ
	texture->setImage(image);
	texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
	stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	return stateset;
}
//��ȡ����ͼ
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
		//��������ͼ��6�������ͼ
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX.get());
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX.get());
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY.get());
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY.get());
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ.get());
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ.get());
		//����������ģʽ
		cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
		//�����˲������κ�mipmap
		cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	}
	return cubemap.get();
}
//����������ͼ����
struct TexMatCallback:public osg::NodeCallback 
{
public:
	//�������
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
			//�õ�ģ����ͼ����������ת�Ƕ�
			const osg::Matrix& MV = *(cv->getModelViewMatrix());
			const osg::Matrix R = osg::Matrix::rotate(osg::DegreesToRadians(112.0f), 0.0f, 0.0f, 1.0f)*
				osg::Matrix::rotate(osg::DegreesToRadians(90.0f), 1.0f, 0.0f, 0.0f);
			osg::Quat q = MV.getRotate();
			const osg::Matrix C = osg::Matrix::rotate(q.inverse());
			//�����������
			_texMat.setMatrix(C*R);
		}
	}
};
//һ���任�࣬ʹ��պ����ӵ���ת
class MoveEarthSkyWithPointTranform:public osg::Transform
{
public:
	//�ֲ����������������
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
	//����������Ϊ�ֲ�����
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
//������պ�
osg::ref_ptr<osg::Node> createSkyBox()
{
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//��������ӳ�䷽ʽ��ָ��Ϊ�����ʽ���������е���ɫ����ԭ������ɫ
	osg::ref_ptr<osg::TexEnv> te = new osg::TexEnv;
	te->setMode(osg::TexEnv::REPLACE);
	stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//�Զ������������꣬���䷽ʽ(REFLECTION_MAP)
	/*
	NORMAL_MAP ��׼ģʽ-����ͼ����
	REFLECTION_MAP ����ģʽ-��������
	SPHERE_MAP ����ģ��-��������
	*/
	osg::ref_ptr<osg::TexGen> tg=new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);
	stateset->setTextureAttributeAndModes(0,tg,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	//�����������
	osg::ref_ptr<osg::TexMat> tm = new osg::TexMat;
	stateset->setTextureAttribute(0, tm);
	//��������ͼ����
	osg::ref_ptr<osg::TextureCubeMap> skymap = readCubeMap();
	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
	//���������ΪԶƽ��
	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0, 1.0);//Զƽ��
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//������Ⱦ˳��Ϊ-1������Ⱦ
	stateset->setRenderBinDetails(-1, "RenderBin");
	osg::ref_ptr<osg::Drawable> drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 1));
	//��������뵽Ҷ�ڵ�
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setCullingActive(false);
	geode->setStateSet(stateset);
	geode->addDrawable(drawable);
	//���ñ任
	osg::ref_ptr<osg::Transform> tranform = new MoveEarthSkyWithPointTranform();
	tranform->setCullingActive(false);
	tranform->addChild(geode);
	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setCullCallback(new TexMatCallback(*tm));
	clearNode->addChild(tranform);
	return clearNode;
}
//����������ƻص�
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
			//���Image������
			int column_start = _image->s() / 4;
			int column_end = 3 * column_start;

			int row_start = _image->t() / 4;
			int row_end = 3 * row_start;
			//���������ݽ��з���
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
			//���Image������
			int column_start = _image->s() / 4;
			int column_end = 3 * column_start;

			int row_start = _image->t() / 4;
			int row_end = 3 * row_start;

			//���������ݽ��з���
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
//����Ԥ��Ⱦ����
osg::ref_ptr<osg::Node> createPreRenderSubGraph(osg::ref_ptr<osg::Node> subgraph,
	unsigned tex_width,
	unsigned tex_height,
	osg::Camera::RenderTargetImplementation renderImplementation,
	bool useImage)
{
	if (!subgraph) return 0;
	//����һ������Ԥ��Ⱦcamera��Group�ڵ�
	osg::ref_ptr<osg::Group> parent = new osg::Group;
	//�������������������Ⱦ�Ľ��
	osg::ref_ptr<osg::Texture> texture = 0;
	{
		osg::ref_ptr<osg::Texture2D> texture2D = new osg::Texture2D;
		texture2D->setTextureSize(tex_width, tex_height);
		texture2D->setInternalFormat(GL_RGBA);
		texture2D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		texture2D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		texture = texture2D;
	}
	//����һ������������ı��μ�����
	{
		osg::ref_ptr<osg::Geometry> polyGeom = new osg::Geometry;
		//���øü����岻������ʾ�б�
		polyGeom->setSupportsDisplayList(false);
		float height = 100.0f;
		float width = 200.0f;
		//�����������飬���������
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
		vertices->push_back(osg::Vec3(width, 0.0f, 0.0f));
		vertices->push_back(osg::Vec3(width, 0.0f, height));
		vertices->push_back(osg::Vec3(0.0f, 0.0f, height));
		//�����������飬���������
		osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
		texcoords->push_back(osg::Vec2(0.0f, 0.0f));
		texcoords->push_back(osg::Vec2(1.0f, 0.0f));
		texcoords->push_back(osg::Vec2(1.0f, 1.0f));
		texcoords->push_back(osg::Vec2(0.0f, 1.0f));
		polyGeom->setVertexArray(vertices);
		//ʹ��vbo��չ
		{
			osg::ref_ptr<osg::VertexBufferObject> vbObject = new osg::VertexBufferObject;
			vertices->setVertexBufferObject(vbObject);
			polyGeom->setUseVertexBufferObjects(true);
		}
		polyGeom->setTexCoordArray(0, texcoords);
		//������ɫ���飬�����ð󶨷�ʽ���������
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		polyGeom->setColorArray(colors);
		polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
		polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vertices->size()));
		//������Ҫ�������ӵ��ü������ϣ�����һ�������������Stateset
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
		polyGeom->setStateSet(stateset);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(polyGeom);
		parent->addChild(geode);
	}
	//��Ҫ����һ������ڵ㣬������Ⱦ��������RTT��
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		//���ñ���ɫ�������ɫ����Ȼ���
		camera->setClearColor(osg::Vec4(0.1f, 0.1f, 0.3f, 1.0f));
		camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//��øýڵ�ķ�Χ��
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
		//����ͶӰ����
		camera->setProjectionMatrixAsFrustum(-proj_right, proj_right, -proj_top, proj_top, znear, zfar);
		//�������׼���ӳ���
		camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		camera->setViewMatrixAsLookAt(bs.center() - osg::Vec3(0.0f, 2.0f, 0.0f)*bs.radius(), bs.center(), osg::Vec3(0.0f, 0.0f, 1.0f));
		//�����ӿ�
		camera->setViewport(0, 0, tex_width, tex_height);
		//�����������Ⱦ����
		camera->setRenderOrder(osg::Camera::PRE_RENDER);
		//���������Ⱦ��ͨ��OpenGL frame buffer object ʵ��
		camera->setRenderTargetImplementation(renderImplementation);
		if (useImage)
		{
			osg::ref_ptr<osg::Image> image = new osg::Image;
			//image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
			image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_FLOAT);
			//��Image���ӵ������COLOR_BUFFER
			camera->attach(osg::Camera::COLOR_BUFFER, image);
			//�������Ļ��ƺ�ص����޸�images����
			camera->setPostDrawCallback(new MyCameraPostDrawCallBack(image));
			//���ﲻֱ�ӽ������COLOR_BUFFER���ӵ�����������Ϊ���޸���Ⱦ���ͼ������
			texture->setImage(0, image);
		}
		else
		{
			//ֱ�ӽ��������ӵ��������ɫ����
			camera->attach(osg::Camera::COLOR_BUFFER, texture);
		}
		//���Ҫ���Ƶ��ӳ���
		camera->addChild(subgraph);
		parent->addChild(camera);
	}
	return parent;
}
//����һά��������
osg::ref_ptr<osg::StateSet> createTexture1DState1()
{
	//������ͼ����ʵ������һ���߶�Ϊ1�Ķ�άͼ��
	osg::ref_ptr<osg::Image> image = new osg::Image;
	//ΪImage����һ���ռ�
	image->allocateImage(1024, 1, 1, GL_RGBA, GL_FLOAT);
	//��������ͼ�����ݸ�ʽRGBA
	image->setInternalTextureFormat(GL_RGBA);
	//ΪImage�������
	osg::Vec4* dataPtr = (osg::Vec4*)image->data();
	for (int i = 0; i < 1024;++i)
	{
		*dataPtr++ = osg::Vec4(1.0f, 0.5f, 0.8f, 0.5f);
	}
	//����һά����
	osg::ref_ptr<osg::Texture1D> texture = new osg::Texture1D;
	//���û���ģʽ
	texture->setWrap(osg::Texture1D::WRAP_S, osg::Texture1D::MIRROR);
	//�����˲�
	texture->setFilter(osg::Texture1D::MIN_FILTER, osg::Texture1D::LINEAR);
	//������ͼ
	texture->setImage(image);
	//�����Զ��������꣬��ָ����ص�ƽ��
	osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen;
	texgen->setMode(osg::TexGen::OBJECT_LINEAR);
	texgen->setPlane(osg::TexGen::S, osg::Plane(0.0f, 0.0f, 1.0f, -10000));
	//�������Լ�
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//����һά����
	stateset->setTextureAttribute(0, texture, osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_1D, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//��������������
	stateset->setTextureAttribute(0, texgen, osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	return stateset;
}
//��ʼ��һ��ͼ�λ���
class MyGraphicsContext
{
public:
	MyGraphicsContext()
	{
		//����ͼ�λ�������
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		//�������Ͻ�����
		traits->x = 0;
		traits->y = 0;
		//���ÿ�Ⱥ͸߶�
		traits->width = 1;
		traits->height = 1;
		//����˫����
		traits->doubleBuffer = false;
		traits->sharedContext = 0;
		//����pbuffer
		traits->pbuffer = true;
		//����ͼ�λ���
		_gc = osg::GraphicsContext::createGraphicsContext(traits);
		//�������ʧ��
		if (!_gc)
		{
			//����pbufferΪfalse
			traits->pbuffer = false;
			//���´���ͼ�λ���
			_gc = osg::GraphicsContext::createGraphicsContext(traits);
		}
		//�Ƿ��ʼ��
		if (!_gc.valid())
		{
			//��ʼ��
			_gc->realize();
			_gc->makeCurrent();
		}
	}
	bool valid() const{ return _gc.valid() && _gc->isRealized(); }
protected:
private:
	osg::ref_ptr<osg::GraphicsContext> _gc;
};
//�򳡾�����ӹ�Դ
osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Group> lightRoot = new osg::Group;
	lightRoot->addChild(node);
	//��������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);
	//�����Χ��
	osg::BoundingSphere bs;
	node->computeBound();
	bs = node->getBound();
	//����һ��Light����
	osg::ref_ptr<osg::Light> light = new osg::Light;
	light->setLightNum(0);
	//���÷���
	light->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	//����λ��
	light->setPosition(osg::Vec4(bs.center().x(), bs.center().y(), bs.center().z() + bs.radius(), 1.0f));
	//���û��������ɫ
	light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//����ɢ������ɫ
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//���ú�˥��ָ��
	light->setConstantAttenuation(1.0f);
	//��������˥��ָ��
	light->setLinearAttenuation(0.0f);
	//���ö��η�˥��ָ��
	light->setQuadraticAttenuation(0.0f);
	//������Դ
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource;
	lightSource->setLight(light);
	lightRoot->addChild(lightSource);
	return lightRoot;
}
//�����۹�������mipmap��ͼ
osg::ref_ptr<osg::Image> createSpotLightImage(const osg::Vec4& centerColor, const osg::Vec4& backgroudColour, unsigned int size, float power)
{
	//����Image����
	osg::ref_ptr<osg::Image> image = new osg::Image;
	//��̬����һ��size*size��С��image
	image->allocateImage(size, size, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	//���image������Ϊԭ�㣬��ɫ��������˥��
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
//�����۹��״̬����
osg::ref_ptr<osg::StateSet> createSpotLightDecoratorState(unsigned int lightNum, unsigned int textureUnit)
{
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	//����IDΪlightNum�Ĺ���
	stateset->setMode(GL_LIGHT0 + lightNum, osg::StateAttribute::ON);
	//�������ĵ���ɫ�ͻ��������ɫ
	osg::Vec4 centerColour(1.0f, 1.0f,1.0f, 1.0f);
	osg::Vec4 ambientColour(0.05f, 0.05f, 0.05f, 1.0f);
	//�����۹������
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(createSpotLightImage(centerColour, ambientColour, 64, 1.0));
	texture->setBorderColor(osg::Vec4(ambientColour));
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
	texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER);
	//������Ԫ
	stateset->setTextureAttributeAndModes(textureUnit, texture, osg::StateAttribute::ON);
	//�����Զ�������������
	stateset->setTextureMode(textureUnit, GL_TEXTURE_GEN_S, osg::StateAttribute::ON);
	stateset->setTextureMode(textureUnit, GL_TEXTURE_GEN_T, osg::StateAttribute::ON);
	stateset->setTextureMode(textureUnit, GL_TEXTURE_GEN_R, osg::StateAttribute::ON);
	stateset->setTextureMode(textureUnit, GL_TEXTURE_GEN_Q, osg::StateAttribute::ON);
	return stateset;
}
//�����۹�ƽڵ�
osg::ref_ptr<osg::Node> createSpotLightNode(const osg::Vec3& position, const osg::Vec3& direction, float angle, unsigned int lightNum, unsigned int textureUnit)
{
	osg::ref_ptr<osg::Group> group = new osg::Group;
	//������Դ
	osg::ref_ptr<osg::LightSource> lightsource = new osg::LightSource;
	osg::ref_ptr<osg::Light> light = lightsource->getLight();
	light->setLightNum(lightNum);
	light->setPosition(osg::Vec4(position,1.0f));
	light->setAmbient(osg::Vec4(0.00f, 0.00f, 0.05f, 1.0f));
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	group->addChild(lightsource);
	//���㷨����
	osg::Vec3 up(0.0f, 0.0f, 1.0f);
	up = (direction^up) ^ direction;
	up.normalize();
	//�����Զ�������������ڵ�
	osg::ref_ptr<osg::TexGenNode> texgenNode = new osg::TexGenNode;
	//��������Ԫ
	texgenNode->setTextureUnit(textureUnit);
	//������������������
	osg::ref_ptr<osg::TexGen> texgen = texgenNode->getTexGen();
	//����ģʽΪ�Ӿ�����
	texgen->setMode(osg::TexGen::EYE_LINEAR);
	//����ͼ��ָ���ο�ƽ��
	texgen->setPlanesFromMatrix(osg::Matrixd::lookAt(position, position + direction, up)* osg::Matrixd::perspective(angle, 1.0, 0.1, 100));
	group->addChild(texgenNode);
	return group;
}
//��������·��
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
//��������ƽ��
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
//��������ģ��
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
		//��Ӿ۹ⶥ�ڵ�
		xform->addChild(createSpotLightNode(osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 1.0f, -1.0f), 60.0f, 0, 1));
		model->addChild(xform);
	}
	return model;
}
//��������
//osg::ref_ptr<osg::Node> createModel()
//{
//	osg::Vec3 center(0.0f, 0.0f, 0.0f);
//	float radius = 100.0f;
//	//��������ģ��
//	osg::ref_ptr<osg::Node> shadower = createMovingModel(center, radius*0.5f);
//	//��������ƽ��
//	osg::ref_ptr<osg::Node> shadowed = createBase(center - osg::Vec3(0.0f, 0.0f, radius*0.1), radius);
//	//����������ڵ�
//	osg::ref_ptr<osg::Group> root = new osg::Group;
//	//����״̬����
//	root->setStateSet(createSpotLightDecoratorState(0, 1));
//	//����ӽڵ�
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
	//����ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");
	root = createClipNode(node);

	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //��Ⱦ״̬
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ȡ��ͼ�ļ�
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	osg::ref_ptr<osg::Node> node = createNode();
	//����״̬������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture2DState(image);
	//ʹ�ö�ά����
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
#endif //��ά����ӳ��
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//��ȡ��ͼ
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	if (image)
	{
		//������ά����
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		texture->setImage(image);
		//�����Զ���������
		osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen;
		texgen->setMode(osg::TexGen::SPHERE_MAP);
		//������������ģʽΪBLEND
		osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
		texenv->setMode(osg::TexEnv::BLEND);
		//����BLEND��������ɫ
		texenv->setColor(osg::Vec4(0.6f, 0.6f, 0.6f, 0.0f));
		//������Ԫ1�Զ������������꣬��ʹ������
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		stateset->setTextureAttributeAndModes(1, texture, osg::StateAttribute::ON);
		stateset->setTextureAttributeAndModes(1, texgen, osg::StateAttribute::ON);
		//����������
		stateset->setTextureAttribute(1, texenv);
		//��������״̬
		node->setStateSet(stateset);
	}
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif // ��������ӳ��
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//����һ��ƽ��
	osg::ref_ptr<osg::Geode> geode = createQuard();
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	osg::ref_ptr<osg::Image> image = new osg::Image;
	//����һ��MipmapDataType�б�������Ÿ���ͼƬ���ݵ�ƫ�Ƶ�ַ
	osg::Image::MipmapDataType mipmapData;
	//����ĳߴ�����ֵ������Ϊ2���ݴ�
	unsigned int s = 256;
	//����������������Ĵ�С
	unsigned int totalSize = 0;
	for (unsigned int i = 0; s > 0; s >>= 1, ++i)
	{
		if (i > 0)
		{
			mipmapData.push_back(totalSize);
		}
		totalSize += s*s * 4;
	}
	//����һ������
	unsigned char* ptr = new unsigned char[totalSize];
	//����image�ĳߴ��С�����ݼ����ݸ�ʽ
	image->setImage(256, 256, 256, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, ptr, osg::Image::USE_NEW_DELETE, 1);
	//��ƫ�Ƶ�ַ����image����
	image->setMipmapLevels(mipmapData);
	//��image������������
	s = 256;
	for (unsigned int i = 0; s > 0; s >>= 1, ++i)
	{
		fillImage(ptr, s);
		ptr += s*s * 4;
	}
	//����һ����ά�������
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	//������ͼ
	texture->setImage(0, image);
	//���ñ߽紦��ΪREPEATE
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	//�����˲�
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST_MIPMAP_NEAREST);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
	//���ö�ά�������
	stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	geode->setStateSet(stateset);
	root->addChild(geode);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif //MipMap����ӳ��
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ȡ��ͼ�ļ�
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	osg::ref_ptr<osg::Node> node = createNode();
	//����״̬������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture2DState1(image);
	//ʹ�ö�ά����
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //TextureRectangle����ӳ��
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");
	//�Զ�����������������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture1DState();
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->realize();
	viewer->run();
	return 0;
#endif // �Զ�������������
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("dumptruck.osg");
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/primitives.gif");
	//������������
	TexCoordGenerator tcg;
	node->accept(tcg);
	//����״̬������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture2DState2(image);
	//ʹ�ö�ά����
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //������������
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> rootnode = new osg::Group;
	//������պ�
	rootnode->addChild(createSkyBox());
	viewer->setSceneData(rootnode);
	viewer->realize();
	viewer->run();
	return 0;
#endif //����ͼ����
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	unsigned tex_width = 1024;
	unsigned tex_height = 512;
	osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
	bool useImage = false;
	//��ȡģ��
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("cessna.osg");
	//����һ��transform�ڵ㣬����ѡװ���ӳ���
	osg::ref_ptr<osg::MatrixTransform> loadedModelTransform = new osg::MatrixTransform;
	loadedModelTransform->addChild(loadedModel);
	//���ø��»ص�
	osg::ref_ptr<osg::NodeCallback> nc = new osg::AnimationPathCallback(loadedModelTransform->getBound().center(), osg::Vec3(0.0f, 0.0f, 1.0f), osg::inDegrees(45.0f));
	loadedModelTransform->setUpdateCallback(nc);
	osg::ref_ptr<osg::Group> rootNode = new osg::Group;
	rootNode->addChild(createPreRenderSubGraph(loadedModelTransform, tex_width, tex_height, renderImplementation, useImage));
	viewer->setSceneData(rootNode);
	viewer->run();
	return 0;
#endif //��Ⱦ������
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ȡģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");
	//ʹ��һά��������
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = createTexture1DState1();
	node->setStateSet(stateset);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //һά����
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ȡģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	//�򳡾�����ӹ�Դ
	root->addChild(createLight(node));
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�򵥹�Դ
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createModel());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //�۹��
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��ȡģ��
	osg::ref_ptr<osg::Node> node = createNode();
	//�õ�״̬����
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = node->getOrCreateStateSet();
	//�������ʶ���
	osg::ref_ptr<osg::Material> mat = new osg::Material;
	//��������ɢ����ɫ
	mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//�������澵����ɫ
	mat->setSpecular(osg::Material::FRONT, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//��������ָ��
	mat->setShininess(osg::Material::FRONT, 90.0f);
	stateset->setAttribute(mat);
	//���ñ����޳�
	osg::ref_ptr<osg::CullFace> cullface = new osg::CullFace(osg::CullFace::BACK);
	stateset->setAttribute(cullface);
	stateset->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //����




}