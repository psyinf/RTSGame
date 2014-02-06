#pragma once
#include <osgWidget/WindowManager>
#include <osgWidget/Box>
#include <osgWidget/Input>
#include <osgWidget/ViewerEventHandlers>

namespace nsGameCore{

class GameCore;

struct ColorLabel: public osgWidget::Label 
{
    ColorLabel(const char* label);

    bool mousePush(double, double, const osgWidget::WindowManager*);

    bool mouseEnter(double, double, const osgWidget::WindowManager*);

    bool mouseLeave(double, double, const osgWidget::WindowManager*);
};

class ColorLabelMenu: public ColorLabel 
{
    osg::ref_ptr<osgWidget::Window> _window;

public:
    ColorLabelMenu(const char* label);

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

	void show(const std::string& name, bool on_off);
protected:
	std::map<std::string, osg::ref_ptr<osgWidget::Widget>> mNamedWidgets;
	osg::ref_ptr<osgWidget::Box> mMainBox;
	GameCore& mrGameCore;
	osg::ref_ptr<osgWidget::WindowManager> mWindowManager;
};

} //namespace