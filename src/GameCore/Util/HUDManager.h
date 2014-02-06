#pragma once
#include <osgWidget/WindowManager>
#include <osgWidget/Box>
#include <osgWidget/Input>
#include <osgWidget/ViewerEventHandlers>
#include <boost/function.hpp>

namespace nsGameCore{

class GameCore;

struct ColorLabel: public osgWidget::Label 
{
    ColorLabel(const std::string& label, const boost::function<void()>& button_pressed_callback);
	
	ColorLabel(const std::string& label);

	void setup( const std::string& label );

    bool mousePush(double, double, const osgWidget::WindowManager*);

    bool mouseEnter(double, double, const osgWidget::WindowManager*);

    bool mouseLeave(double, double, const osgWidget::WindowManager*);
protected:
	boost::function<void()> mButtonPressedCallback;
};

class ColorLabelMenu: public ColorLabel 
{
    osg::ref_ptr<osgWidget::Window> _window;

public:
    ColorLabelMenu(const std::string& label);

	void addEntry(const std::string& text, boost::function<void()>& button_pressed );

    void managed(osgWidget::WindowManager* wm);

    void positioned();

    bool mousePush(double, double, const osgWidget::WindowManager*);

    bool mouseLeave(double, double, const osgWidget::WindowManager*);
};


class HUDManager
{
public:
	HUDManager(GameCore& game_core);

	~HUDManager();

	void createSplashScreen();

	ColorLabelMenu* getMenu();

	void show(const std::string& name, bool on_off);
protected:
	std::map<std::string, osg::ref_ptr<osgWidget::Widget>> mNamedWidgets;
	osg::ref_ptr<osgWidget::Box> mMainBox;
	GameCore& mrGameCore;
	osg::ref_ptr<osgWidget::WindowManager> mWindowManager;
};

} //namespace