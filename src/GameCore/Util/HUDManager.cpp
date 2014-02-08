#pragma once
#include "HUDManager.h"
#include "../GameCore.h"
#include "Common/Config.h"
#include "Core/Core.h"

bool nsGameCore::NamedLabel::mouseLeave( double, double, const osgWidget::WindowManager* )
{
	setColor(0.0f, 0.0f, 0.3f, 0.4f);
	
	return true;
}

bool nsGameCore::NamedLabel::mouseEnter( double, double, const osgWidget::WindowManager* )
{
	setColor(0.0f, 0.6f, 0.3f, 0.4f);

	return true;
}

bool nsGameCore::NamedLabel::mousePush( double, double, const osgWidget::WindowManager* )
{
	getParent()->hide();
	mButtonPressedCallback();
	return true;
}

nsGameCore::NamedLabel::NamedLabel( const std::string& label,  const boost::function<void()>&  button_press_callback ) 
	:osgWidget::Label("", "")
	,mButtonPressedCallback(button_press_callback)

{
	setup(label);

	
}

nsGameCore::NamedLabel::NamedLabel( const std::string& label) 
	:osgWidget::Label("", "")
{
	setup(label);

}

void nsGameCore::NamedLabel::setup( const std::string& label )
{
	setImage("./data/ui/textures/theme-1.png", true);
	setFont("./data/fonts/Vera.ttf");
	setFontSize(20);
	setFontColor(1.0f, 1.0f, 1.0f, 1.0f);
	setColor(0.0f, 0.0f, 0.3f, 0.4f);
	addHeight(28.0f);
	addWidth(150.0);
	setCanFill(true);
	setLabel(label);
	setEventMask(osgWidget::EVENT_MOUSE_PUSH | osgWidget::EVENT_MASK_MOUSE_MOVE);
}

bool nsGameCore::LabelMenu::mouseLeave( double, double, const osgWidget::WindowManager* )
{
	if(!_window->isVisible()) setColor(0.8f, 0.8f, 0.8f, 0.1f);

	return true;
}

bool nsGameCore::LabelMenu::mousePush( double, double, const osgWidget::WindowManager* )
{
	if(!_window->isVisible()) _window->show();

	else _window->hide();

	return true;
}

void nsGameCore::LabelMenu::positioned()
{
	osgWidget::Label::positioned();

	_window->setOrigin(getX(), getHeight());
	_window->resize(getWidth());
}

void nsGameCore::LabelMenu::managed( osgWidget::WindowManager* wm )
{
	osgWidget::Label::managed(wm);

	wm->addChild(_window.get());

	_window->hide();
}

nsGameCore::LabelMenu::LabelMenu( const std::string&  label ) 
	:NamedLabel(label)
{
	_window = new osgWidget::Box(
		std::string("Menu_") + label,
		osgWidget::Box::VERTICAL,
		true
		);

	_window->getBackground()->setColor(0,0,0,0);
	setFont("./data/fonts/Vera.ttf");
	_window->resize();
	setImage("./data/ui/textures/theme-1.png", true, true);
	//setColor(0.8f, 0.8f, 0.8f, 0.1f);
}

void nsGameCore::LabelMenu::addEntry( const std::string& text, boost::function<void()>& button_pressed )
{
	_window->addWidget(new NamedLabel(text, button_pressed));
	_window->show();
	_window->resize();
	_window->hide();
}

void nsGameCore::HUDManager::show( const std::string& name, bool on_off )
{
	if (on_off)
	{
		mMainBox->show();
	}
	else
	{
		mMainBox->hide();
	}
}

void nsGameCore::HUDManager::createSplashScreen()
{
	osg::ref_ptr<osgWidget::Widget> fullscreen_widget = new osgWidget::Widget("Fullscreen",1,1);
	fullscreen_widget->setCanFill(true);
	fullscreen_widget->setColor(osgWidget::Color(0.1,0.1,0.1,0.9));
	fullscreen_widget->setImage("./data/ui/textures/theme-2.png", true, true);
	mMainBox->addWidget(fullscreen_widget);
	mMainBox->resizePercent(100,100);

	mMainBox->resize();
}

nsGameCore::HUDManager::~HUDManager()
{

}

nsGameCore::HUDManager::HUDManager( GameCore& game_core ) :mrGameCore(game_core)
{
	osgViewer::Viewer* viewer = game_core.getRenderCore().getViewerRef();
	nsRenderer::Config config = game_core.getRenderCore().getConfigRef();

	mWindowManager = new osgWidget::WindowManager(
		viewer,
		config.mScene.mOffscreenResolutionX,
		config.mScene.mOffscreenResolutionY,
		0xf0000000,
		osgWidget::WindowManager::WM_PICK_DEBUG);

	mMainBox   = new osgWidget::Box("vbox", osgWidget::Box::HORIZONTAL);

	//box->setOrigin(200.0f, 200.0f);

	osg::Camera* camera = osgWidget::createOrthoCamera(config.mScene.mOffscreenResolutionX, config.mScene.mOffscreenResolutionY);
	camera->addChild(mWindowManager);

	viewer->addEventHandler(new osgWidget::MouseHandler(mWindowManager));
	//viewer->addEventHandler(new osgWidget::KeyboardHandler(mWindowManager));
	viewer->addEventHandler(new osgWidget::ResizeHandler(mWindowManager, camera));
	viewer->addEventHandler(new osgWidget::CameraSwitchHandler(mWindowManager, camera));
	//viewer.addEventHandler(new osgViewer::WindowSizeHandler());

	mWindowManager->addChild(mMainBox);
	mMainBox->getBackground()->setColor(0.0,0.0,0.0,0.0);
	//mMainBox->getBackground()->setImage("./data/ui/textures/theme-2.png", true);
	//createSplashScreen();
	mWindowManager->resizeAllWindows();
	//TODO: This fucks up the scene and camera management. 
	mrGameCore.getRenderCore().getMainRoot()->addChild(camera);
}

nsGameCore::LabelMenu* nsGameCore::HUDManager::getMenu(const std::string& menu_name)
{
	if (mNamedWidgets.count(menu_name))
	{
		return dynamic_cast<nsGameCore::LabelMenu*>(mNamedWidgets[menu_name].get());
	}
	return 0;
	
}

void nsGameCore::HUDManager::addMenu( const std::string& menu_name )
{
	if (!mNamedWidgets.count(menu_name))
	{
		osg::ref_ptr<osgWidget::Label> label =  new LabelMenu(menu_name);
		mNamedWidgets[menu_name] = label;
		mMainBox->addWidget(label);
	}
}
