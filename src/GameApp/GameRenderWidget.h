#pragma once

//#include <Common/LightManager.h>


#if defined(_MSC_VER) && defined(OSG_DISABLE_MSVC_WARNINGS)
// disable warning "'QtConcurrent::BlockSizeManager' : assignment operator could not be generated"
#pragma warning( disable : 4512 )
#endif



#include <osgViewer/api/Win32/GraphicsWindowWin32>


using WindowHandle = HWND;
using WindowData = osgViewer::GraphicsWindowWin32::WindowData;

#include <osgGA/StateSetManipulator>
#include <osgDB/ReadFile>

#include <RenderCore/RenderCore.h>

#include <iostream>
#include <sstream>


namespace nsGameCore
{
	class GameCore;
}
// ---------


//	OSG core includes




#include <osgViewer/Viewer>


//	Qt stuff

#include <QApplication>
#include <QTimer>
#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QtGui)
QT_FORWARD_DECLARE_CLASS(QCalendarWidget)
QT_FORWARD_DECLARE_CLASS(QKeyEvent)
QT_FORWARD_DECLARE_CLASS(QInputEvent)
QT_FORWARD_DECLARE_CLASS(QString)
QT_FORWARD_DECLARE_CLASS(QtGui)
QT_FORWARD_DECLARE_CLASS(QWidget)

using Qt::WindowFlags;




class ViewerWidget : public QWidget
{
	Q_OBJECT

public:
	osg::ref_ptr<osg::Group>	getModelRoot();

	osg::Camera*				getCamera() const;

	const std::string&			getResourcePath();

	bool			setupScene(const std::string& model_name);
	
	QTimer mQTimer;

	void keyPressEvent(QKeyEvent * event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
public:
    ViewerWidget( QWidget* parent, const char* name =0, WindowFlags flags =0, bool overrideTraits = false);
    void setup();
    // Prevent flicker on Windows Qt
	QPaintEngine* paintEngine() const override
	{ 
		return nullptr; 
	}

	

protected:
    
	void createContext();
	
	

public slots:
	void updateFrame();
	
private:
	std::shared_ptr<renderer::RenderCore>			mRenderCore;
	std::shared_ptr<nsGameCore::GameCore>			mGameCore;
	
	osg::ref_ptr<osgViewer::GraphicsWindow> mOsgGraphicsWindow;

    osgGA::EventQueue* getEventQueue() const;
    void               setKeyboardModifiers(QInputEvent* event);
    bool               mbOverrideTraits;
	
};
