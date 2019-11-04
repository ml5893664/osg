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

//创建文字
osg::ref_ptr<osg::Geode> createText()
{
	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	osg::ref_ptr<osgText::Font> font = new osgText::Font;
	//读取文字
	font = osgText::readFontFile("simhei.ttf");
	//设置字体文件
	text->setFont(font);
	//设置文字信息
	text->setText(L"http://www.OsgChina.org--OpenSceneGraph 中国官方");
	//设置字体大小
	text->setCharacterSize(0.5f);
	text->setAutoRotateToScreen(true);
	//设置字体颜色
	text->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//设置显示的位置
	osg::Vec3f position = osg::Vec3f(0.0f, -10.0f, 0.0f);
	text->setPosition(position);
	//设置对齐方式
	text->setAlignment(osgText::Text::CENTER_TOP);
	//设置旋转方式
	text->setAxisAlignment(osgText::Text::SCREEN);
	//添加到叶节点中
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(text);
	return geode;
}

osg::ref_ptr<osg::Camera> createAllKindText()
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	//设置投影矩阵
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1280, 0, 800));
	//设置视图矩阵
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	//清除深度缓存
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	//设置渲染顺序，在主摄像机之前渲染
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	//设置为不接收事件，始终不得到焦点
	camera->setAllowEventFocus(false);
	osg::ref_ptr<osgText::Font> font=osgText::readFontFile("fonts/arial.ttf");
	osg::ref_ptr<osg::Geode> geode=new osg::Geode;
	osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	//设置文字输出的格式

	//设置格式为LEFT_TO_RIGHT,从左到右
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
	//设置格式为RIGHT_TO_LEFT,从右到左
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
	//设置格式为VERTICAL,垂直
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

	//设置阴影
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(font);
		text->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text->setCharacterSize(40.0f);
		text->setPosition(osg::Vec3(100.0f, 650.0f, 0.0f));
		//设置阴影为DROP_SHADOW_BOTTOM_RIGHT
		text->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);
		text->setBackdropColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
		text->setBackdropOffset(0.1f, -0.1f);
		text->setText("text->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);");
		geode->addDrawable(text);
	}

	//设置边框
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(font);
		text->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text->setCharacterSize(40.0f);
		text->setPosition(osg::Vec3(100.0f, 250.0f, 0.0f));
		//设置颜色倾斜模式为PER_CHARACTER
		text->setColorGradientMode(osgText::Text::PER_CHARACTER);
		//设置倾斜4个角落的颜色
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
	//设置投影矩阵
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1280, 0, 800));
	//设置视图矩阵，同时确保不被场景中其他图形位置变换影响，使用绝对帧引用
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	//清除深度缓存
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	//设置渲染顺序为POST
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	//设置为不接收事件，始终不得到焦点
	camera->setAllowEventFocus(false);
	osg::ref_ptr<osg::Geode> geode=new osg::Geode;
	osg::ref_ptr<osg::StateSet> stateset=geode->getOrCreateStateSet();
	//关闭光照
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//关闭深度测试
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	//设置文字
	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	osg::ref_ptr<osgText::Font> font = new osgText::Font;
	font = osgText::readFontFile("simhei.ttf");
	text->setFont(font);
	text->setText(L"www.osgChina.org--OpenSceneGraph 中国官方");
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
	//创建渐变文字对象
	osg::ref_ptr<osgText::FadeText> text = new osgText::FadeText;
	//设置筛选回调
	osg::Vec3 normal(0.0f, 0.0f, 1.0f);
	//设置为聚集筛选，一种类似于背面筛选的场景筛选方法，但是它可以将多个对象组合起来并进行统一的背面筛选
	text->setCullCallback(new osg::ClusterCullingCallback(pos,normal,0.0));
	text->setText(str);
	text->setFont("fonts/airal.ttf");
	text->setPosition(pos);
	text->setCharacterSize(300.0f);
	//设置字体大小的格式
	text->setCharacterSizeMode(osgText::Text::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
	text->setAutoRotateToScreen(true);
	return text;
}

//创建渐变文字
osg::ref_ptr<osg::Node> createFadeText()
{
	osg::ref_ptr<osg::Group> group = new osg::Group;
	//关闭深度测试
	group->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addChild(geode);
	//设置TEXT
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
	//添加Drawable
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
	//创建三维文字
	osg::ref_ptr<osgText::Text3D> text = new osgText::Text3D;
	text->setText(L"http://www.OsgChina.org--OpenSceneGraph 中国官方");
	text->setFont("simhei.ttf");
	text->setCharacterSize(60.0f);
	text->setPosition(osg::Vec3(0.0f,0.0f,0.0f));
	//设置文字渲染模式
	text->setRenderMode(osgText::Text3D::PER_GLYPH);
	//设置文字深度
	text->setCharacterDepth(10.0f);
	text->setDrawMode(osgText::Text3D::TEXT|osgText::Text3D::BOUNDINGBOX);
	//设置文字与坐标轴对齐方式
	text->setAxisAlignment(osgText::Text3D::XZ_PLANE);
	geode->addDrawable(text);
	//设置材质
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
	//创建文字
	osg::ref_ptr<osg::Geode> node = createText();
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //显示汉字
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createAllKindText());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //各种文字效果
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("logo.ive");
	root->addChild(node);
	//添加HUD文字
	root->addChild(createHUDText());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //HUD显示汉字
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createFadeText());
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("lz.osg");
	root->addChild(node);
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //渐变色文字
#if 0
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(create3DText());
	viewer->setSceneData(root);
	viewer->run();
	return 0;
#endif //3D汉字显示




}