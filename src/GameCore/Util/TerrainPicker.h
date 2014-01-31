#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
namespace osgTerrain
{
	class TerrainTile;
}

namespace nsGameCore{

class GameCore;
// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler {
public:
	osg::Geode* createFaceSelector();
	PickHandler(nsGameCore::GameCore& game_core);

	~PickHandler();

	bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	void moveTerrain(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
	void placeModel(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	void highlightSelected( osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	void getFace(const osgUtil::LineSegmentIntersector::Intersection& intersection);

	void getAreaFaces(const osgUtil::LineSegmentIntersector::Intersection& intersection, osgTerrain::TerrainTile*);
protected:
	osg::ref_ptr<osg::Geometry> mSelectionGeometry;
	nsGameCore::GameCore& mrGameCore;

	bool mRightMousePressed;
	bool mLeftMousePressed;
};

}