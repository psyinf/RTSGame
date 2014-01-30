#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
namespace nsGameCore{

class GameCore;
// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler {
public:

	PickHandler(nsGameCore::GameCore& game_core);

	~PickHandler();

	bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	void moveTerrain(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
	void placeModel(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	  

protected:
	nsGameCore::GameCore& mrGameCore;
};

}