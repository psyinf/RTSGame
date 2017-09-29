#include "GameGUI.h"

#include <osgViewer/View>
 
#include <boost/assign.hpp>

#include <Core/EnvironmentInterface.h>

#include "GameRenderWidget.h"

#include <QtGui>

using namespace boost::assign;

DesktopMainWindow::DesktopMainWindow(nsRenderer::Config& config, std::string& filename, bool viewer_mode, int dock_pos)
: mConfig(config)
, mModelToLoadByArg(filename)
, ui(new Ui::PilotDriverGUI)
, mFullScreen(viewer_mode)
{
	initUi();

	setMouseTracking(true);
	mOsgViewerWidget->setMouseTracking(true);

	this->setMouseTracking(true);
//	this->setWindowFlags(Qt::FramelessWindowHint);
// 	setGeometry(0,0,1920,1200);
// 	showFullScreen();
//	QWidget::setGeometry(QApplication::desktop()->availableGeometry());

	loadAndSetStylesheet();




	QString app_path = QCoreApplication::applicationDirPath();	
	QString style1;

	QFile data1("./resources/stylesheets/PDG_MDFS.qss");
	if(data1.open(QFile::ReadOnly))
	{
		QTextStream styleIn(&data1);
		style1 = styleIn.readAll();
	}	

	style1.replace("../resources", "./resources", Qt::CaseInsensitive);
	style1.replace("/*##APPPATH##*/", app_path + "/resources/stylesheets");		

	data1.close();



	if (mFullScreen)
	{
		setWindowState(Qt::WindowFullScreen);
		showFullScreen();
	}

	
	

	
	
//	this->showFullScreen();
}

void DesktopMainWindow::loadAndSetStylesheet()
{

 	QString app_path = QCoreApplication::applicationDirPath();	

	QFile data("./resources/stylesheets/main_style.qss");
	QString style;
	if(data.open(QFile::ReadOnly))
	{
		QTextStream styleIn(&data);
		style = styleIn.readAll();
	}
	style.replace("../resources", "./resources", Qt::CaseInsensitive);
	style.replace("/*##APPPATH##*/", app_path + "/resources/stylesheets");		

	setStyleSheet(style);
	data.close();




}

void DesktopMainWindow::initUi()
{
	ui->setupUi(this);
	
	mOsgViewerWidget = new ViewerWidget(mConfig, this, 0, 0, false );
	
 	this->centralWidget()->setLayout(new QVBoxLayout);

	setCentralWidget(mOsgViewerWidget);
	
}