#pragma once

#include <Common/LightManager.h>


#if defined(_MSC_VER) && defined(OSG_DISABLE_MSVC_WARNINGS)
// disable warning "'QtConcurrent::BlockSizeManager' : assignment operator could not be generated"
#pragma warning( disable : 4512 )
#endif



#include <osgViewer/api/Win32/GraphicsWindowWin32>


typedef HWND WindowHandle;
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData;

#include <osgGA/StateSetManipulator>
#include <osgDB/ReadFile>

#include <Core/Core.h>
#include <iostream>
#include <sstream>

namespace nsRenderer
{
	class SceneData;
}
namespace nsGameCore
{
	class GameCore;
}
// ---------


//	OSG core includes
#include <osg/CullFace>
#include <osg/Depth>
#include <osg/FrontFace>
#include <osg/ClampColor>
#include <osg/Geode>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Stencil>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgText/Text>

#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/FlightManipulator>
#include <osgViewer/ViewerEventHandlers>

//	OSG extra includes
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>	
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osg/PositionAttitudeTransform>
#include <osg/PolygonMode>
#include <osg/BlendFunc>
#include <osg/Switch>
#include <osgUtil/Optimizer>
#include <osgSim/Impostor>

#include <osgShadow/ShadowedScene>
#include <osgShadow/LightSpacePerspectiveShadowMap>

#include <osgViewer/CompositeViewer>
#include <osgViewer/Viewer>
#include <osgViewer/Renderer>

//	Qt stuff
#include <osgQt/GraphicsWindowQt>
#include <QtGui/QApplication>
#include <QTimer>

QT_FORWARD_DECLARE_CLASS(QtGui)
QT_FORWARD_DECLARE_CLASS(QCalendarWidget)
QT_FORWARD_DECLARE_CLASS(QKeyEvent)
QT_FORWARD_DECLARE_CLASS(QString)
QT_FORWARD_DECLARE_CLASS(QtGui)
QT_FORWARD_DECLARE_CLASS(QWidget)

using Qt::WindowFlags;


//	silverlining stuff
//#include "SilverLining/SkySetup.h"
//#include "SilverLining/AtmosphereReference.h"

//	other stuff
#include "Common/CommonHelpers.h"
#include "Common/FindAlphaVisitor.h"
#include "Common/BindTexturesVisitor.h"
#include "Common/FindLightsVisitor.h"
#include "Common/FindNodeVisitor.h"
#include "Common/NodeHelpers.h"
#include <Common/RenderPass.h>
#include "Common/SpaceMouseManipulator.h"
#include "Common/SceneSupportData.h"

#include <Filesystem/Directory.h>
//#include <Common/PickHandler.h>
//----------


class ViewerWidget : public QWidget
{
	Q_OBJECT

public:
	osg::ref_ptr<osg::Group>	getModelRoot();

    osgViewer::View*			getView() const;

	osg::Camera*				getCamera() const;

	const std::string&			getResourcePath();

	boost::shared_ptr<nsRenderer::EnvironmentInterface>		getSkyEnv();

	bool			setupScene(const std::string& model_name);
	
	QTimer mQTimer;


public:
    ViewerWidget(const nsRenderer::Config& config, QWidget* parent, const char* name =0, WindowFlags flags =0, bool overrideTraits = false);

    // Prevent flicker on Windows Qt
	QPaintEngine* paintEngine() const 
	{ 
		return 0; 
	}

	void setConfig(const nsRenderer::Config& config);

protected:

    void init();
    
	void createContext();
	
	osg::ref_ptr<osgViewer::GraphicsWindow> mOsgGraphicsWindow;
    
    bool mbOverrideTraits;

public slots:
	void updateFrame();
	
private:
	boost::shared_ptr<nsRenderer::Core>				mRenderCore;
	boost::shared_ptr<nsGameCore::GameCore>			mGameCore;
	

	nsRenderer::Config mConfig;	
};
