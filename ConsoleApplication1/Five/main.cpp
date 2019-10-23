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


}