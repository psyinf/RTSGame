#pragma once

#include <QMainWindow>
#include <QTime>



//#include <QtUiTools/QUiLoader>

#include "GameRenderWidget.h"

#include "ui_GameGUI.h"



class DesktopMainWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		DesktopMainWindow( std::string& filename, bool viewer_mode, int dock_pos);
		
	private:
		Ui::PilotDriverGUI* ui;

		void initUi();
		void replaceDefaultStylesheetValues( QString &style );
		void loadAndSetStylesheet();


		ViewerWidget* mOsgViewerWidget;

		osg::ref_ptr<osg::Group> mRoot;

		std::string mModelToLoadByArg;
		bool mFullScreen;

};


