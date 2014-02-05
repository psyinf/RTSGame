#pragma once
#include <osgWidget/WindowManager>
#include <osgWidget/Box>
#include <osgWidget/Input>
#include <osgWidget/ViewerEventHandlers>

namespace nsGameCore{


class HUDManager
{
public:
	HUDManager(GameCore& game_core)
		:mrGameCore(game_core)
	{
		osgViewer::Viewer* viewer = game_core.getRenderCore().getViewerRef();
		mWindowManager = new osgWidget::WindowManager(
			viewer,
			1280.0f,
			1024.0f,
			0xf0000000,
			osgWidget::WindowManager::WM_PICK_DEBUG);

		osgWidget::Box*   box   = new osgWidget::Box("vbox", osgWidget::Box::VERTICAL);
		osgWidget::Input* input = new osgWidget::Input("input", "", 50);

		input->setFont("fonts/VeraMono.ttf");
		input->setFontColor(0.0f, 0.0f, 0.0f, 1.0f);
		input->setFontSize(15);
		input->setYOffset(input->calculateBestYOffset("y"));
		input->setSize(400.0f, input->getText()->getCharacterHeight());

		box->addWidget(input);
		box->setOrigin(200.0f, 200.0f);
		osg::Camera* camera = mWindowManager->createParentOrthoCamera();

		viewer->addEventHandler(new osgWidget::MouseHandler(mWindowManager));
		viewer->addEventHandler(new osgWidget::KeyboardHandler(mWindowManager));
		viewer->addEventHandler(new osgWidget::ResizeHandler(mWindowManager, camera));
		viewer->addEventHandler(new osgWidget::CameraSwitchHandler(mWindowManager, camera));
		//viewer.addEventHandler(new osgViewer::WindowSizeHandler());

		mWindowManager->addChild(box);
		mWindowManager->resizeAllWindows();
		//TODO: This fucks up the scene and camera management. 
		mrGameCore.getRenderCore().getMainRoot()->addChild(camera);
	}

	~HUDManager()
	{}

	//TODO: use osgWidget?


protected:
	GameCore& mrGameCore;
	osg::ref_ptr<osgWidget::WindowManager> mWindowManager;
};

} //namespace