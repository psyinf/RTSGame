#include "GameRenderWidget.h"

#include <GameCore/GameCore.h>
#include <RenderCore/RenderCore.h>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QMessageBox>

#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgViewer/ViewerEventHandlers>


class QtKeyboardMap
{

public:
	QtKeyboardMap()
	{
		mKeyMap[Qt::Key_Escape] = osgGA::GUIEventAdapter::KEY_Escape;
		mKeyMap[Qt::Key_Delete] = osgGA::GUIEventAdapter::KEY_Delete;
		mKeyMap[Qt::Key_Home] = osgGA::GUIEventAdapter::KEY_Home;
		mKeyMap[Qt::Key_Enter] = osgGA::GUIEventAdapter::KEY_KP_Enter;
		mKeyMap[Qt::Key_End] = osgGA::GUIEventAdapter::KEY_End;
		mKeyMap[Qt::Key_Return] = osgGA::GUIEventAdapter::KEY_Return;
		mKeyMap[Qt::Key_PageUp] = osgGA::GUIEventAdapter::KEY_Page_Up;
		mKeyMap[Qt::Key_PageDown] = osgGA::GUIEventAdapter::KEY_Page_Down;
		mKeyMap[Qt::Key_Left] = osgGA::GUIEventAdapter::KEY_Left;
		mKeyMap[Qt::Key_Right] = osgGA::GUIEventAdapter::KEY_Right;
		mKeyMap[Qt::Key_Up] = osgGA::GUIEventAdapter::KEY_Up;
		mKeyMap[Qt::Key_Down] = osgGA::GUIEventAdapter::KEY_Down;
		mKeyMap[Qt::Key_Backspace] = osgGA::GUIEventAdapter::KEY_BackSpace;
		mKeyMap[Qt::Key_Tab] = osgGA::GUIEventAdapter::KEY_Tab;
		mKeyMap[Qt::Key_Space] = osgGA::GUIEventAdapter::KEY_Space;
		mKeyMap[Qt::Key_Delete] = osgGA::GUIEventAdapter::KEY_Delete;
		mKeyMap[Qt::Key_Alt] = osgGA::GUIEventAdapter::KEY_Alt_L;
		mKeyMap[Qt::Key_Shift] = osgGA::GUIEventAdapter::KEY_Shift_L;
		mKeyMap[Qt::Key_Control] = osgGA::GUIEventAdapter::KEY_Control_L;
		mKeyMap[Qt::Key_Meta] = osgGA::GUIEventAdapter::KEY_Meta_L;

		mKeyMap[Qt::Key_F1] = osgGA::GUIEventAdapter::KEY_F1;
		mKeyMap[Qt::Key_F2] = osgGA::GUIEventAdapter::KEY_F2;
		mKeyMap[Qt::Key_F3] = osgGA::GUIEventAdapter::KEY_F3;
		mKeyMap[Qt::Key_F4] = osgGA::GUIEventAdapter::KEY_F4;
		mKeyMap[Qt::Key_F5] = osgGA::GUIEventAdapter::KEY_F5;
		mKeyMap[Qt::Key_F6] = osgGA::GUIEventAdapter::KEY_F6;
		mKeyMap[Qt::Key_F7] = osgGA::GUIEventAdapter::KEY_F7;
		mKeyMap[Qt::Key_F8] = osgGA::GUIEventAdapter::KEY_F8;
		mKeyMap[Qt::Key_F9] = osgGA::GUIEventAdapter::KEY_F9;
		mKeyMap[Qt::Key_F10] = osgGA::GUIEventAdapter::KEY_F10;
		mKeyMap[Qt::Key_F11] = osgGA::GUIEventAdapter::KEY_F11;
		mKeyMap[Qt::Key_F12] = osgGA::GUIEventAdapter::KEY_F12;
		mKeyMap[Qt::Key_F13] = osgGA::GUIEventAdapter::KEY_F13;
		mKeyMap[Qt::Key_F14] = osgGA::GUIEventAdapter::KEY_F14;
		mKeyMap[Qt::Key_F15] = osgGA::GUIEventAdapter::KEY_F15;
		mKeyMap[Qt::Key_F16] = osgGA::GUIEventAdapter::KEY_F16;
		mKeyMap[Qt::Key_F17] = osgGA::GUIEventAdapter::KEY_F17;
		mKeyMap[Qt::Key_F18] = osgGA::GUIEventAdapter::KEY_F18;
		mKeyMap[Qt::Key_F19] = osgGA::GUIEventAdapter::KEY_F19;
		mKeyMap[Qt::Key_F20] = osgGA::GUIEventAdapter::KEY_F20;

		mKeyMap[Qt::Key_hyphen] = '-';
		mKeyMap[Qt::Key_Equal] = '=';

		mKeyMap[Qt::Key_division] = osgGA::GUIEventAdapter::KEY_KP_Divide;
		mKeyMap[Qt::Key_multiply] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
		mKeyMap[Qt::Key_Minus] = '-';
		mKeyMap[Qt::Key_Plus] = '+';

		mKeyMap[Qt::Key_Insert] = osgGA::GUIEventAdapter::KEY_KP_Insert;
		//mKeyMap[Qt::Key_Delete        ] = osgGA::GUIEventAdapter::KEY_KP_Delete;
	}

	~QtKeyboardMap()
	{
	}

	int remapKey(QKeyEvent* event)
	{
		KeyMap::iterator itr = mKeyMap.find(event->key());
		if (itr == mKeyMap.end())
		{
			return int(*(event->text().toLatin1().data()));
		}
		else
			return itr->second;
	}

private:
	using KeyMap = std::map<unsigned int, int>;
	KeyMap mKeyMap;
};
static QtKeyboardMap sQtKeyBoardMap;

void ViewerWidget::keyPressEvent(QKeyEvent * event)
{
//	setKeyboardModifiers(event);
	int value = sQtKeyBoardMap.remapKey(event);
	getEventQueue()->keyPress(value);
}

void ViewerWidget::keyReleaseEvent(QKeyEvent* event)
{
//	setKeyboardModifiers(event);
	int value = sQtKeyBoardMap.remapKey(event);
	getEventQueue()->keyRelease(value);
}

ViewerWidget::ViewerWidget( QWidget* parent, const char* name, WindowFlags f, bool overrideTraits) 
	:QWidget(parent, f)
	,mbOverrideTraits (overrideTraits)
{
	try 
	{
        mRenderCore = std::make_shared<renderer::RenderCore>();
		// set up the OpenGL context for an OSG window
//		setLayout(new QHBoxLayout);
		createContext();

		// configure Qt to render the OSG window
		setAttribute(Qt::WA_PaintOnScreen);
		setAttribute(Qt::WA_NoSystemBackground);
		setFocusPolicy(Qt::ClickFocus);

		setContextMenuPolicy(Qt::CustomContextMenu);
		setAcceptDrops(true);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(this, "Error","Error setting up ViewerWidget: \n" + QString(e.what()) );
	}
}

void ViewerWidget::createContext()
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    traits->readDISPLAY();
    if (traits->displayNum<0) 
	{
		traits->displayNum = 0;
	}

    traits->windowName = "GameApp";
	traits->screenNum = 0; // mConfig.mWindowProps.mScreen;
	traits->x = 0; // mConfig.mWindowProps.mStartX;;
	traits->y = 0;// mConfig.mWindowProps.mStartY;;
	traits->width = 1024;// mConfig.mScene.mOffscreenResolutionX;
	traits->height = 1024;// mConfig.mScene.mOffscreenResolutionY;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

	traits->inheritedWindowData = new WindowData((HWND)(winId()));



    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    mOsgGraphicsWindow = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());



	mRenderCore->getViewer()->addEventHandler(new osgViewer::StatsHandler);
    mRenderCore->getViewer()->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
	    
	mRenderCore->getViewer()->getCamera()->setGraphicsContext( mOsgGraphicsWindow );
    mRenderCore->getViewer()->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
    mRenderCore->getViewer()->addEventHandler(new osgGA::StateSetManipulator());
    mRenderCore->getViewer()->addEventHandler(new osgViewer::ThreadingHandler());

	mRenderCore->getViewer()->setKeyEventSetsDone(0);
    mRenderCore->getViewer()->setQuitEventSetsDone(false);
    mRenderCore->getViewer()->getCamera()->setAllowEventFocus(true);
	
	osgGA::KeySwitchMatrixManipulator* keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
	keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
    mRenderCore->getViewer()->setCameraManipulator(keyswitchManipulator);


	mRenderCore->setup();




	connect( &mQTimer, SIGNAL(timeout()), this, SLOT(updateFrame()) );
	mQTimer.start( 1 );
	setFocus( Qt::ActiveWindowFocusReason );
	
	
}


osgGA::EventQueue* ViewerWidget::getEventQueue() const
{
	osgGA::EventQueue* eventQueue = mOsgGraphicsWindow->getEventQueue();

	if (eventQueue)
	{
		return eventQueue;
	}
	else
	{
		throw std::runtime_error("Unable to obtain valid event queue");
	}
}

void ViewerWidget::setKeyboardModifiers(QInputEvent* event) const
{

	int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier);
	unsigned int mask = 0;
	if (modkey & Qt::ShiftModifier) mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
	if (modkey & Qt::ControlModifier) mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;
	if (modkey & Qt::AltModifier) mask |= osgGA::GUIEventAdapter::MODKEY_ALT;
	getEventQueue()->getCurrentEventState()->setModKeyMask(mask);
}

bool ViewerWidget::setupScene( const std::string& model_name ) const
{
    osg::ref_ptr<osg::Node> loaded_scene = osgDB::readNodeFile(model_name);
    
	if (loaded_scene)
	{
		return true;
	}
	return false;
}



const std::string& ViewerWidget::getResourcePath() const 
{
	//TODO: return resource path from configuration
    static std::string respath = "data";
    return respath;
}


void ViewerWidget::updateFrame()
{
	if (!mGameCore)
	{
		try
		{
			mGameCore = std::make_shared<nsGameCore::GameCore>(*mRenderCore);
			mGameCore->setup("GameConfig.ini");
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error initializing Core: " << e.what() << std::endl;
		}
		
	}
	try 
	{
		mGameCore->frame();
		mRenderCore->frame();
	}
	catch (const std::exception& e)
	{
		std::cout << "Frame failed: " << e.what() << std::endl;
		mQTimer.stop();
	}
}

osg::ref_ptr<osg::Group> ViewerWidget::getModelRoot()
{
	return mRenderCore->getSubRoot("TERRAIN_ROOT");
}

osg::Camera* ViewerWidget::getCamera() const
{
	 return mRenderCore->getViewer()->getCamera();
}


void ViewerWidget::setup()
{
	try 
	{
		// set up the OpenGL context for an OSG window
		setLayout(new QHBoxLayout);
		createContext();

		// configure Qt to render the OSG window
		setAttribute(Qt::WA_PaintOnScreen);
		setAttribute(Qt::WA_NoSystemBackground);
		setFocusPolicy(Qt::ClickFocus);

		setContextMenuPolicy(Qt::CustomContextMenu);
		setAcceptDrops(true);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(this, "Error","Error setting up ViewerWidget: \n" + QString(e.what()) );
	}
}

