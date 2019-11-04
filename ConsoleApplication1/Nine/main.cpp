#include <osgViewer/Viewer>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Camera>
#include <osg/Matrix>
#include <osg/ClusterCullingCallback>
#include <osg/Material>
#include <osg/StateSet>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgText/Text>
#include <osgText/Font>
#include <osgText//FadeText>
#include <osgText/Text3D>
#include <osgText/Font3D>

#include <osgUtil/Optimizer>

#include <iostream>

//��������
osg::ref_ptr<osg::Geode> createText()
{
	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	osg::ref_ptr<osgText::Font> font = new osgText::Font;
	//��ȡ����
	font = osgText::readFontFile("simhei.ttf");
	//���������ļ�
	text->setFont(font);
	//����������Ϣ
	text->setText(L"http://www.OsgChina.org--OpenSceneGraph �й��ٷ�");
	//���������С
	text->setCharacterSize(0.5f);
	text->setAutoRotateToScreen(true);
	//����������ɫ
	text->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//������ʾ��λ��
	osg::Vec3f position = osg::Vec3f(0.0f, -10.0f, 0.0f);
	text->setPosition(position);
	//���ö��뷽ʽ
	text->setAlignment(osgText::Text::CENTER_TOP);
	//������ת��ʽ
	text->setAxisAlignment(osgText::Text::SCREEN);
	//��ӵ�Ҷ�ڵ���
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(text);
	return geode;
}

osg::ref_ptr<osg::Camera> createAllKindText()
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	//����ͶӰ����
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1280, 0, 800));
	//������ͼ����
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	//�����Ȼ���
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	//������Ⱦ˳�����������֮ǰ��Ⱦ
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	//����Ϊ�������¼���ʼ�ղ��õ�����
	camera->setAllowEventFocus(false);
	osg::ref_ptr<osgText::Font> font=osgText::readFontFile("fonts/arial.ttf");
	osg::ref_ptr<osg::Geode> geode=new osg::Geode;
	osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	//������������ĸ�ʽ

	//���ø�ʽΪLEFT_TO_RIGHT,������
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(font);
		text->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text->setCharacterSize(20.0f);
		text->setPosition(osg::Vec3(50.0f, 750.0f, 0.0f));
		text->setLayout(osgText::Text::LEFT_TO_RIGHT);
		text->setText("text->setLayout(osgText::Text::LEFT_TO_RIGHT);");
		geode->addDrawable(text);
	}
	//���ø�ʽΪRIGHT_TO_LEFT,���ҵ���
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(font);
		text->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text->setCharacterSize(30.0f);
		text->setPosition(osg::Vec3(1200.0f, 750.0f, 0.0f));
		text->setLayout(osgText::Text::RIGHT_TO_LEFT);
		text->setAlignment(osgText::Text::RIGHT_BASE_LINE);
		text->setText("text->setLayout(osgText::Text::RIGHT_TO_LEFT);");
		geode->addDrawable(text);
	}
	//���ø�ʽΪVERTICAL,��ֱ
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(font);
		text->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text->setCharacterSize(20.0f);
		text->setPosition(osg::Vec3(50.0f, 750.0f, 0.0f));
		text->setLayout(osgText::Text::VERTICAL);
		text->setText("text->setLayout(osgText::Text::VERTICAL);");
		geode->addDrawable(text);
	}

	//������Ӱ
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(font);
		text->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text->setCharacterSize(40.0f);
		text->setPosition(osg::Vec3(100.0f, 650.0f, 0.0f));
		//������ӰΪDROP_SHADOW_BOTTOM_RIGHT
		text->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);
		text->setBackdropColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
		text->setBackdropOffset(0.1f, -0.1f);
		text->setText("text->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);");
		geode->addDrawable(text);
	}

	//���ñ߿�
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(font);
		text->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text->setCharacterSize(40.0f);
		text->setPosition(osg::Vec3(100.0f, 250.0f, 0.0f));
		//������ɫ��бģʽΪPER_CHARACTER
		text->setColorGradientMode(osgText::Text::PER_CHARACTER);
		//������б4���������ɫ
		text->setColorGradientCorners(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f), osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f),
			osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f), osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		text->setText("text->setBackdropType(osgText::Text::SOLID);");
		geode->addDrawable(text);
	}
	camera->addChild(geode);
	return camera;
}

osg::ref_ptr<osg::Camera> createHUDText()
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	//����ͶӰ����
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1280, 0, 800));
	//������ͼ����ͬʱȷ����������������ͼ��λ�ñ任Ӱ�죬ʹ�þ���֡����
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	//�����Ȼ���
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	//������Ⱦ˳��ΪPOST
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	//����Ϊ�������¼���ʼ�ղ��õ�����
	camera->setAllowEventFocus(false);
	osg::ref_ptr<osg::Geode> geode=new osg::Geode;
	osg::ref_ptr<osg::StateSet> stateset=geode->getOrCreateStateSet();
	//�رչ���
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//�ر���Ȳ���
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	//��������
	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	osg::ref_ptr<osgText::Font> font = new osgText::Font;
	font = osgText::readFontFile("simhei.ttf");
	text->setFont(font);
	text->setText(L"www.osgChina.org--OpenSceneGraph �й��ٷ�");
	text->setPosition(osg::Vec3(100.0f, 600.0f, 0.0f));
	text->setCharacterSize(40.0f);
	text->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	text->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
	geode->addDrawable(text);
	camera->addChild(geode);
	return camera;
}

osg::ref_ptr<osgText::Text> createText1(const std::string& str, osg::Vec3 pos)
{
	//�����������ֶ���
	osg::ref_ptr<osgText::FadeText> text = new osgText::FadeText;
	//����ɸѡ�ص�
	osg::Vec3 normal(0.0f, 0.0f, 1.0f);
	//����Ϊ�ۼ�ɸѡ��һ�������ڱ���ɸѡ�ĳ���ɸѡ���������������Խ���������������������ͳһ�ı���ɸѡ
	text->setCullCallback(new osg::ClusterCullingCallback(pos,normal,0.0));
	text->setText(str);
	text->setFont("fonts/airal.ttf");
	text->setPosition(pos);
	text->setCharacterSize(300.0f);
	//���������С�ĸ�ʽ
	text->setCharacterSizeMode(osgText::Text::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
	text->setAutoRotateToScreen(true);
	return text;
}

//������������
osg::ref_ptr<osg::Node> createFadeText()
{
	osg::ref_ptr<osg::Group> group = new osg::Group;
	//�ر���Ȳ���
	group->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addChild(geode);
	//����TEXT
	std::vector<std::string> textList;
	textList.push_back("1");
	textList.push_back("2");
	textList.push_back("3");
	textList.push_back("4");
	textList.push_back("5");
	textList.push_back("6");
	textList.push_back("7");
	textList.push_back("8");
	textList.push_back("9");
	textList.push_back("10");
	int numX = 200;
	int delta = 10;
	//���Drawable
	int t = 0;
	for (int i = 100; i < numX;i+=delta,t++)
	{
		geode->addDrawable(createText1(textList[t%textList.size()], osg::Vec3(0, i, 100)));
	}
	return group;
}

osg::ref_ptr<osg::Geode> create3DText()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//������ά����
	osg::ref_ptr<osgText::Text3D> text = new osgText::Text3D;
	text->setText(L"http://www.OsgChina.org--OpenSceneGraph �й��ٷ�");
	text->setFont("simhei.ttf");
	text->setCharacterSize(60.0f);
	text->setPosition(osg::Vec3(0.0f,0.0f,0.0f));
	//����������Ⱦģʽ
	text->setRenderMode(osgText::Text3D::PER_GLYPH);
	//�����������
	text->setCharacterDepth(10.0f);
	text->setDrawMode(osgText::Text3D::TEXT|osgText::Text3D::BOUNDINGBOX);
	//������������������뷽ʽ
	text->setAxisAlignment(osgText::Text3D::XZ_PLANE);
	geode->addDrawable(text);
	//���ò���
	osg::ref_ptr<osg::Material> front = new osg::Material;
	front->setColorMode(osg::Material::AMBIENT);
	front->setAlpha(osg::Material::FRONT_AND_BACK, 1);
	front->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	front->setDiffuse(osg::Material::FRONT_AND_BACK, osg:: Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	geode->getOrCreateStateSet()->setAttributeAndModes(front);
	return geode;
}

int main()
{
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	//��������
	osg::ref_ptr<osg::Geode> node = createText();
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //��ʾ����
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createAllKindText());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //��������Ч��
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("logo.ive");
	root->addChild(node);
	//���HUD����
	root->addChild(createHUDText());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //HUD��ʾ����
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createFadeText());
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //����ɫ����
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(create3DText());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //3D������ʾ




}