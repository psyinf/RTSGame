#pragma once
#include "HUDManager.h"
#include "../GameCore.h"
#include "RenderCore/RenderCore.h"

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

nsGameCore::NamedLabel::NamedLabel( const std::string& label,  const std::function<void()>&  button_press_callback ) 
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
	if(!_window->isVisible())
	{	
		setColor(0.8f, 0.8f, 0.8f, 0.1f);
	}
	else
	{
		//_window->hide();
	}
	

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
	,mIsOpen(false)
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
	setColor(0.8f, 0.8f, 0.8f, 0.1f);
}

void nsGameCore::LabelMenu::addEntry( const std::string& text, std::function<void()> button_pressed )
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



nsGameCore::HUDManager::~HUDManager()
{

}

nsGameCore::HUDManager::HUDManager( GameCore& game_core ) :mrGameCore(game_core)
{
    osgViewer::Viewer* viewer = mrGameCore.getRenderCore().getViewer();
	//XXX nsRenderer::Config config = game_core.getRenderCore().getConfigRef();

	mWindowManager = new osgWidget::WindowManager(
		viewer,
		2048, //XXX config.mScene.mOffscreenResolutionX, 
		2048,//XXX config.mScene.mOffscreenResolutionY,
		0xffff0000,
		osgWidget::WindowManager::WM_PICK_DEBUG);

	


	//box->setOrigin(200.0f, 200.0f);

	osg::Camera* camera = osgWidget::createOrthoCamera(2048, 2048);
	camera->addChild(mWindowManager);
	
	viewer->addEventHandler(new osgWidget::MouseHandler(mWindowManager));
	//viewer->addEventHandler(new osgWidget::KeyboardHandler(mWindowManager));
	viewer->addEventHandler(new osgWidget::ResizeHandler(mWindowManager, camera));
	viewer->addEventHandler(new osgWidget::CameraSwitchHandler(mWindowManager, camera));
	//viewer.addEventHandler(new osgViewer::WindowSizeHandler());
	
	mMainBox   = new osgWidget::Box("vbox", osgWidget::Box::HORIZONTAL);
	
	mWindowManager->addChild(mMainBox);
	mMainBox->getBackground()->setColor(0.0,0.0,0.0,0.0);

	mStatusBox = new osgWidget::Box("Status Bar", osgWidget::Box::HORIZONTAL);
	//mStatusBox->setAnchorVertical(osgWidget::Window::VA_TOP);
	mWindowManager->addChild(mStatusBox);
	

	mWindowManager->resizeAllWindows();
	//TODO: This fucks up the scene and camera management. 
	 mrGameCore.getRenderCore().getSubRoot("MAIN_ROOT")->addChild(camera);
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

nsGameCore::NameValueLabel* nsGameCore::HUDManager::addNameValueLabel( const std::string& name )
{
	nsGameCore::NameValueLabel* nv_label = new NameValueLabel(name);
	mNameValueLabels[name] = nv_label;
	mStatusBox->addWidget(nv_label);
	
	
	return nv_label;
}

nsGameCore::NameValueLabel* nsGameCore::HUDManager::getNameValueLabel( const std::string& name )
{
	if (mNameValueLabels.count(name))
	{
		return (mNameValueLabels[name].get());
	}
	throw std::runtime_error("Unknown NameValueLabel object:" + name);
}

void nsGameCore::NameValueLabel::positioned()
{
	osgWidget::Label::positioned();

	mWindow->setOrigin(getX(), getHeight());
	mWindow->resize(getWidth());
}

void nsGameCore::NameValueLabel::managed( osgWidget::WindowManager* wm )
{
	osgWidget::Label::managed(wm);

	wm->addChild(mWindow.get());
}

osgWidget::Label* nsGameCore::NameValueLabel::getValueLabel()
{
	return mValueLabel;
}

osgWidget::Label* nsGameCore::NameValueLabel::getNameLabel()
{
	return mNameLabel;
}

void nsGameCore::NameValueLabel::setLabelValue( const std::string& label_value )
{
	mValueLabel->setLabel(label_value);
}

void nsGameCore::NameValueLabel::setLabelName( const std::string& label_name )
{
	mNameLabel->setLabel(label_name);
}

nsGameCore::NameValueLabel::NameValueLabel( const std::string& element_name ) 
	:osgWidget::Label(element_name, osgWidget::Box::HORIZONTAL)
{
	mWindow = new osgWidget::Box(
		std::string("Menu_") + element_name,
		osgWidget::Box::HORIZONTAL,
		true
		);

	mWindow->getBackground()->setColor(0,0,0,0);
	//setFont("./data/fonts/Vera.ttf");

	setImage("./data/ui/textures/theme-1.png", true, true);

	mNameLabel = new osgWidget::Label("LabelName", element_name);
	mValueLabel = new osgWidget::Label("LabelValue", "");
	mWindow->addWidget(mNameLabel);
	mWindow->addWidget(mValueLabel);
	//addHeight(28.0f);
	addWidth(250.0);
	mWindow->setAnchorVertical(osgWidget::Window::VA_TOP);
	//set font
	//XXX mNameLabel->setFont("./data/fonts/Data_Control.ttf");
	//XXX mValueLabel->setFont("./data/fonts/Data_Control.ttf");
	mWindow->getBackground()->setColor(0,0,0,0);
	mWindow->resize();
}
