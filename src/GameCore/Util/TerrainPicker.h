#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <boost/shared_ptr.hpp>
namespace osgTerrain
{
	class TerrainTile;
}

namespace nsGameCore{

struct CellAdress;
class GameCore;

// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler 
{
	struct PickInfo
	{
		PickInfo()
			:mTerrainTile()
			,mIntersection()
		{

		}

		bool pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);


		osg::ref_ptr<osgTerrain::TerrainTile>		  mTerrainTile;
		osgUtil::LineSegmentIntersector::Intersection mIntersection;
	};

public:
	osg::Geode* createFaceSelector();
	PickHandler(nsGameCore::GameCore& game_core);

	~PickHandler();

	bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	void levelTerrain(const osgUtil::LineSegmentIntersector::Intersection& intersection, osgTerrain::TerrainTile* terrain_tile );

	void moveTerrain(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
	void placeModel(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	void highlightSelected();

	void setSelectionColor(const osg::Vec4& color );

	void pickTerrain( osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	void getFace(const osgUtil::LineSegmentIntersector::Intersection& intersection);

	void getAreaFaces(const osgUtil::LineSegmentIntersector::Intersection& intersection, osgTerrain::TerrainTile*);

	osg::Vec3d getClampedPosition(const osg::Vec3d& pos, unsigned int x_scale, unsigned int y_scale) const;

	nsGameCore::CellAdress getCellAdress(osgTerrain::TerrainTile* terrain_tile, osg::Vec3d world_position);

protected:
	PickInfo mCurrentPickInfo;

	osg::ref_ptr<osg::Geometry> mSelectionGeometry;
	
	nsGameCore::GameCore& mrGameCore;

	bool mRightMousePressed;
	bool mLeftMousePressed;
};

}