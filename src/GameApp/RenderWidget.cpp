#include "GameRenderWidget.h"

#include <QtGui/QDesktopServices>
#include <QHBoxLayout>
#include <QMessageBox>
// #include <QTextStream>
// #include <QUrl>

#include <osg/ValueObject>
#include <osgParticle/ParticleEffect>
#include <osgUtil/SmoothingVisitor>

#include <boost/assign.hpp>
#include <boost/bind.hpp>

#include <iomanip>



#include <Common/SceneData.h>
#include <Common/RenderPass.h>
#include <Common/PickHandler.h>
#include <Common/SceneSupportData.h>
#include <Common/GetHotList.h>
#include <Common/Material.h>
#include <Core/MakeInstancedVisitor.h>
#include <Common/EarthManipulator.h>
#include <Core/GeometryManager.h>
#include <Core/EnvironmentInterface.h>
#include <GameCore/GameCore.h>



using namespace boost::assign;

#define USE_QT4

ViewerWidget::ViewerWidget(const nsRenderer::Config& config, QWidget* parent, const char* name, WindowFlags f, bool overrideTraits) 
	:QWidget(parent, f)
	,mConfig(config)
	,mbOverrideTraits (overrideTraits)
	,mRenderCore()
{
	try 
	{
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

    traits->windowName = "osgViewerQt";
    traits->screenNum = mConfig.mWindowProps.mScreen;
    traits->x = mConfig.mWindowProps.mStartX;;
    traits->y = mConfig.mWindowProps.mStartY;;
    traits->width = mConfig.mScene.mOffscreenResolutionX;
    traits->height =mConfig.mScene.mOffscreenResolutionY;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    traits->inheritedWindowData = new WindowData((HWND)winId());

    if (ds->getStereo())
    {
        switch(ds->getStereoMode())
        {
        case(osg::DisplaySettings::QUAD_BUFFER): traits->quadBufferStereo = true; break;
        case(osg::DisplaySettings::VERTICAL_INTERLACE):
        case(osg::DisplaySettings::CHECKERBOARD):
        case(osg::DisplaySettings::HORIZONTAL_INTERLACE): traits->stencil = 8; break;
        default: break;
        }
    }

    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    mOsgGraphicsWindow = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());

    // get around dearanged traits on X11 (MTCompositeViewer only)
    if (mbOverrideTraits)
    {
        traits->x = x();
        traits->y = y();
        traits->width = width();
        traits->height = height();
	}

	mRenderCore.reset(new nsRenderer::Core(mConfig));
	mRenderCore->setStatsHandler(new osgViewer::StatsHandler());
    

    mRenderCore->getViewerRef()->addEventHandler(new osgViewer::LODScaleHandler);

	//mRenderCore->getViewerRef()->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	mRenderCore->getViewerRef()->getCamera()->setGraphicsContext( mOsgGraphicsWindow );
	mRenderCore->getViewerRef()->getCamera()->setViewport( new osg::Viewport(0,0,traits->width,traits->height) );
	mRenderCore->getViewerRef()->addEventHandler( new osgGA::StateSetManipulator() );
	mRenderCore->getViewerRef()->addEventHandler( new osgViewer::ThreadingHandler() );

	mRenderCore->getViewerRef()->setKeyEventSetsDone( 0 );
	mRenderCore->getViewerRef()->setQuitEventSetsDone( false );
	mRenderCore->getViewerRef()->getCamera()->setAllowEventFocus(true);

	//osg::setNotifyLevel( osg::NOTICE );

	osgGA::KeySwitchMatrixManipulator* keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
	keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
	keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
	keyswitchManipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator() );
	keyswitchManipulator->addMatrixManipulator( '4', "Terrain", new osgGA::TerrainManipulator() ); 
	


	mRenderCore->setup();
	mRenderCore->getViewerRef()->setCameraManipulator(keyswitchManipulator);



	connect( &mQTimer, SIGNAL(timeout()), this, SLOT(updateFrame()) );
	mQTimer.start( 1 );
	setFocus( Qt::ActiveWindowFocusReason );
	
	
}


bool ViewerWidget::setupScene( const std::string& model_name )
{
	//TODO: this should be based on a config option or strg/shift modifier
	//mRenderCore->reset();
	osg::ref_ptr<osg::Node> loaded_scene = mRenderCore->loadScene(model_name, true);
	if (loaded_scene)
	{
		return true;
	}
	return false;
}

boost::shared_ptr<nsRenderer::EnvironmentInterface> ViewerWidget::getSkyEnv()
{
	return mRenderCore->getEnvironmentInterface();
}

const std::string& ViewerWidget::getResourcePath()
{
	return mRenderCore->getConfigRef().mEnvironment.mResourcePath;
}

osgViewer::View* ViewerWidget::getView() const
{
	return mRenderCore->getViewerRef().get();
}

void ViewerWidget::updateFrame()
{
	try 
	{
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
	
	return mRenderCore->getSubRoot("TERRAIN_ROOT");//->getParent(0);
	//return mMainRoot;
}

osg::Camera* ViewerWidget::getCamera() const
{
	return mRenderCore->getViewerRef()->getCamera();
}

void ViewerWidget::setConfig( const nsRenderer::Config& config )
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
