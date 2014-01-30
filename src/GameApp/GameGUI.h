#pragma once

#include <QMainWindow>
#include <QTime>

#include <common/Config.h>
#include <Core/Core.h>

#include <QtUiTools/QUiLoader>

#include "GameRenderWidget.h"

#include "ui_GameGUI.h"



class DesktopMainWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		DesktopMainWindow(nsRenderer::Config& config, std::string& filename, bool viewer_mode, int dock_pos);
		
	private:
		Ui::PilotDriverGUI* ui;

		void initUi();
		void replaceDefaultStylesheetValues( QString &style );
		void loadAndSetStylesheet();
		// 		void setScene(const std::string& filename, bool absolute);
// 		void setScene(osg::Node* n);

		ViewerWidget* mOsgViewerWidget;

		osg::ref_ptr<osg::Group> mRoot;

		nsRenderer::Config mConfig;
		std::string mModelToLoadByArg;
		bool mFullScreen;

};


