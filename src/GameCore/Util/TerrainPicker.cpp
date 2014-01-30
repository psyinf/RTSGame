#include "TerrainPicker.h"
#include <GameCore/GameCore.h>
#include <osgTerrain/Terrain>
#include "TerrainTechniques.h"

void nsGameCore::PickHandler::moveTerrain( osgViewer::View* view, const osgGA::GUIEventAdapter& ea )
{
	osgUtil::LineSegmentIntersector::Intersections intersections;

	if (view->computeIntersections(ea,intersections))
	{
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
			hitr != intersections.end();
			++hitr)
		{

			for (osg::NodePath::const_iterator iter = (*hitr).nodePath.begin(); iter != (*hitr).nodePath.end(); ++iter)
			{
				osgTerrain::TerrainTile* terrain_tile = dynamic_cast<osgTerrain::TerrainTile*>((*iter));
				if (terrain_tile)
				{
					osgTerrain::ModifyingTerrainTechnique* mod_technique = dynamic_cast<osgTerrain::ModifyingTerrainTechnique*>(terrain_tile->getTerrainTechnique());
					if (mod_technique)
					{
						osg::Vec3d local_position;
						osgTerrain::Locator* locator = terrain_tile->getElevationLayer()->getLocator();
						if (locator)
						{
							double height_diff = 0.0;
							if ("TERRAIN_UP" == mrGameCore.getCurrentEditMode().getCurrentModeName())
							{
								height_diff= 0.5;
							}
							else if ("TERRAIN_DOWN" == mrGameCore.getCurrentEditMode().getCurrentModeName())
							{
								height_diff=-0.5;
							}
							locator->convertModelToLocal((*hitr).getWorldIntersectPoint(),local_position);
							double height = 0.0;
							mod_technique->getHeight(local_position[0], local_position[1], height);	 
							mod_technique->setHeight(local_position[0], local_position[1], height + height_diff);	 
						}
					}
				}
			}
		}
	}
	//setLabel(gdlist);
}

void nsGameCore::PickHandler::placeModel( osgViewer::View* view, const osgGA::GUIEventAdapter& ea )
{
	osgUtil::LineSegmentIntersector::Intersections intersections;

	if (view->computeIntersections(ea,intersections))
	{
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
			hitr != intersections.end();
			++hitr)
		{

			for (osg::NodePath::const_iterator iter = (*hitr).nodePath.begin(); iter != (*hitr).nodePath.end(); ++iter)
			{
				osgTerrain::TerrainTile* terrain_tile = dynamic_cast<osgTerrain::TerrainTile*>((*iter));
				if (terrain_tile)
				{
					mrGameCore.placeModel((*hitr).getWorldIntersectPoint(), osg::Quat(), "./data/models/default.flt");
				}
			}
		}
	}
	//setLabel(gdlist);
}

bool nsGameCore::PickHandler::handle( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	if (osgGA::GUIEventAdapter::FRAME == ea.getEventType())
	{
		return false;
	}
	bool treat_as_handled =false;
	//determine mode
	bool is_active = (ea. getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT);
	bool is_terrain_mode = is_active && boost::istarts_with(mrGameCore.getCurrentEditMode().getCurrentModeName(),"TERRAIN");
	bool is_place_mode = is_active && boost::istarts_with(mrGameCore.getCurrentEditMode().getCurrentModeName(),"PLACE");
	
	osgText::Text* text = mrGameCore.getNamedTextObject("CurrentMode");
	text->setColor(is_active ? osg::Vec4(1,1,1,1) : osg::Vec4(1,1,1,0.2));

	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::PUSH):
		{
			if (is_terrain_mode)
			{
				osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
				if (view) moveTerrain(view,ea);
				treat_as_handled = true;
			}
			else if (is_place_mode)
			{
				osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
				if (view)
				{
					placeModel(view, ea);
					treat_as_handled = true;
				}
			}

		}
		break;
	case(osgGA::GUIEventAdapter::DRAG):
		{
			if (is_terrain_mode)
			{
				osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
				if (view) moveTerrain(view,ea);
				treat_as_handled = true;
			}
		}
		break;
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (ea.getKey()=='c')
			{
				osgText::Text* txt = mrGameCore.getNamedTextObject("CurrentMode");
				txt->setText(mrGameCore.getCurrentEditMode().nextMode());
				
			}
			return false;
		}
	default:
		break;
	}
	return treat_as_handled;
}

nsGameCore::PickHandler::~PickHandler()
{

}

nsGameCore::PickHandler::PickHandler(nsGameCore::GameCore& game_core)
	:osgGA::GUIEventHandler()
	,mrGameCore(game_core)
{
	osgText::Text* text_node = new osgText::Text();
	text_node->setFont("fonts/arial.ttf");
	text_node->setPosition(osg::Vec3(20.0,500,0.0));
	mrGameCore.createNamedTextObject("CurrentMode", text_node);
}

