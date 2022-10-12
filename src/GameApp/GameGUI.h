#pragma once

#include <QMainWindow>
#include <QTime>

#include "GameRenderWidget.h"

#include "ui_GameGUI.h"



class DesktopMainWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		DesktopMainWindow(const std::string& filename, bool viewer_mode, int dock_pos);
		
	private:
        Ui::RTSGameGUI* ui;

		void initUi();
		void loadAndSetStylesheet();


		ViewerWidget* mOsgViewerWidget;

		osg::ref_ptr<osg::Group> mRoot;

		std::string mModelToLoadByArg;
		bool mFullScreen;

};


