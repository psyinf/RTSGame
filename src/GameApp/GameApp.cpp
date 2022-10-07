#include <iostream>
#include <cstdlib>

#include <QApplication>
#include <QDialog>
#include "GameGUI.h"
#include <boost/program_options.hpp>

int main( int argc, char** argv )
{
	QApplication app(argc, argv);
	QStringList args = app.arguments();

	namespace po = boost::program_options;

	std::string file_name;
	std::string config_name;
	bool viewer_mode;
	int dock_pos;

	// Declare the supported options.
	po::options_description cmd_params("Command line parameters");
	cmd_params.add_options()
		("help,h",																					"produce this help message")
		("model,m",			po::value<std::string>(&file_name),										"the model/scene to load")
		("config-file,c",	po::value<std::string>(&config_name)->default_value("MainViewer.ini"),	"a config file where more options are defined")
		("fullscreen,f",	po::value<bool>(&viewer_mode)->default_value(false),					"run in fullscreen")
		("dockpos,d",		po::value<int>(&dock_pos)->default_value(0),							"dockwidget position")
		;

	po::positional_options_description pos;
	pos.add("model", 1).
		add("config-file", 1).
		add("fullscreen", 1).
		add("dockpos", 1);


	po::variables_map vm;
	po::store( po::command_line_parser(argc, argv).options(cmd_params).positional(pos).run(), vm );
	po::notify(vm);

	if (vm.count("help")) 
	{
		std::cout << cmd_params << "\n";
		return 1;
	}
	//Matrix_Test();

	/* XXX
	//nsRenderer::Config config;
	//config.save("MainViewer.ini");
	
	if (!config.load(config_name))
	{
		config.save(config_name);
	}
	*/
	DesktopMainWindow* dialog = new DesktopMainWindow( file_name, viewer_mode, dock_pos);
	//dialog->setGeometry( config.mWindowProps.mStartX, config.mWindowProps.mStartY, config.mWindowProps.mWidth, config.mWindowProps.mHeight );
	dialog->show();

	return app.exec();
}

