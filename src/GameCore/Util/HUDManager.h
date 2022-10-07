#pragma once
#include <osgWidget/WindowManager>
#include <osgWidget/Box>
#include <osgWidget/Input>
#include <osgWidget/ViewerEventHandlers>

#include <functional>

namespace nsGameCore{

class GameCore;

struct NamedLabel: public osgWidget::Label 
{
    NamedLabel(const std::string& label, const std::function<void()>& button_pressed_callback);
	
	NamedLabel(const std::string& label);

	void setup( const std::string& label );

    bool mousePush(double, double, const osgWidget::WindowManager*);

    bool mouseEnter(double, double, const osgWidget::WindowManager*);

    bool mouseLeave(double, double, const osgWidget::WindowManager*);
protected:
	std::function<void()> mButtonPressedCallback;
};

class LabelMenu: public NamedLabel 
{
protected:
    osg::ref_ptr<osgWidget::Window> _window;
	bool mIsOpen;
public:
    LabelMenu(const std::string& label);

	void addEntry(const std::string& text, std::function<void()> button_pressed );

    void managed(osgWidget::WindowManager* wm);

    void positioned();

    bool mousePush(double, double, const osgWidget::WindowManager*);

    bool mouseLeave(double, double, const osgWidget::WindowManager*);
};

class NameValueLabel : public osgWidget::Label
{
public:
	NameValueLabel(const std::string& element_name);
	void setLabelName(const std::string& label_name);
	void setLabelValue(const std::string& label_value);

	osgWidget::Label* getNameLabel();
	osgWidget::Label* getValueLabel();

	void managed( osgWidget::WindowManager* wm );

	void positioned();
protected:
	osg::ref_ptr<osgWidget::Window> mWindow;
 	osg::ref_ptr<osgWidget::Label> mNameLabel;
 	osg::ref_ptr<osgWidget::Label> mValueLabel;
};


class HUDManager
{
public:
	HUDManager(GameCore& game_core);

	~HUDManager();

	LabelMenu* getMenu(const std::string& menu_name);
	
	void addMenu(const std::string& menu_name);

	NameValueLabel* addNameValueLabel(const std::string& name);

	NameValueLabel* getNameValueLabel(const std::string& name);

	void show(const std::string& name, bool on_off);

	
protected:
	std::map<std::string, osg::ref_ptr<NameValueLabel>>		mNameValueLabels;
	std::map<std::string, osg::ref_ptr<osgWidget::Widget>>	mNamedWidgets;
	osg::ref_ptr<osgWidget::Box> mMainBox;
	osg::ref_ptr<osgWidget::Box> mStatusBox;
	GameCore& mrGameCore;
	osg::ref_ptr<osgWidget::WindowManager> mWindowManager;
};

} //namespace