#include "TerrainPicker.h"
#include <GameCore/GameCore.h>
#include "TerrainTechniques.h"
#include <Core/Core.h>


#include <osgTerrain/Terrain>
#include <osg/PolygonOffset>

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
		//update selection
		highlightSelected(view, ea);
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
}


void nsGameCore::PickHandler::highlightSelected( osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
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
				if (terrain_tile && (*hitr).drawable)
				{
					getFace(*hitr);
					getAreaFaces(*hitr, terrain_tile);
				}
			}
			
		}
	}
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
	//pick triangle
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view)
	{
		return false;
	}
	highlightSelected(view, ea);


	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::PUSH):
		{
			if (ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
			{
				mLeftMousePressed = true;
				std::cout << "left down" << std::endl;
			}
			if (ea.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
			{
				mRightMousePressed = true;
				std::cout << "right down" << std::endl;
			}
			if (is_terrain_mode)
			{
				moveTerrain(view,ea);
				std::cout << "moving";
				treat_as_handled = true;
			}
			else if (is_place_mode)
			{
				placeModel(view, ea);
				treat_as_handled = true;
			}

		}
		break;
	case (osgGA::GUIEventAdapter::RELEASE):
		{
			if (0 == (ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) && mLeftMousePressed)
			{
				mLeftMousePressed = false;
				std::cout << "left up" << std::endl;
			}
			if (0 == (ea.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) && mRightMousePressed)
			{
				mRightMousePressed = false;
				std::cout << "right up" << std::endl;
			}
		}
	break;
	case(osgGA::GUIEventAdapter::DRAG):
		{
			if (is_terrain_mode)
			{
				moveTerrain(view,ea);
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
	osg::Geode* selection_geometry = createFaceSelector();
	mrGameCore.getRenderCore().getSubRoot("MODEL_ROOT")->addChild(selection_geometry);
}

void nsGameCore::PickHandler::getFace(const osgUtil::LineSegmentIntersector::Intersection& intersection )
{
	osg::Geometry* geom = (intersection.drawable->asGeometry());
	if (!geom || geom == mSelectionGeometry || !mSelectionGeometry)
	{
		return;
	}

	osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>( geom->getVertexArray() );
	osg::Vec3Array* selected_vertices = dynamic_cast<osg::Vec3Array*>( mSelectionGeometry->getVertexArray() );
	if ( !vertices || !selected_vertices ) return;

	osg::Matrix matrix = osg::computeLocalToWorld( intersection.nodePath );
	const std::vector<unsigned int>& selIndices = intersection.indexList;
	for ( unsigned int i=0; i<3 && i<selIndices.size(); ++i )
	{
		unsigned int pos = selIndices[i];
		(*selected_vertices)[i] = (*vertices)[pos] * matrix;
	}
	selected_vertices->dirty();
	mSelectionGeometry->dirtyBound();
}

osg::Geode* nsGameCore::PickHandler::createFaceSelector()
{
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	const osg::Vec4 selectedColor(1.0f, 0.0f, 0.0f, 0.5f);
	(*colors)[0] = selectedColor;

	mSelectionGeometry = new osg::Geometry;
	mSelectionGeometry->setDataVariance( osg::Object::DYNAMIC );
	mSelectionGeometry->setUseDisplayList( false );
	mSelectionGeometry->setUseVertexBufferObjects( true );
	mSelectionGeometry->setVertexArray( new osg::Vec3Array(10) );
	mSelectionGeometry->setColorArray( colors.get() );
	mSelectionGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
	mSelectionGeometry->addPrimitiveSet( new osg::DrawArrays(GL_TRIANGLE_FAN, 0, 10) );

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( mSelectionGeometry.get() );
	geode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	geode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	geode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	geode->getOrCreateStateSet()->setAttributeAndModes( new osg::PolygonOffset(-1.0f, -1.1f) );
	return geode.release();
}

void nsGameCore::PickHandler::getAreaFaces( const osgUtil::LineSegmentIntersector::Intersection& intersection
											, osgTerrain::TerrainTile* terrain_tile
											)
{

	struct clampToGrid 
	{
		clampToGrid(unsigned int x_scale, unsigned int y_scale)
			:mXScale(x_scale)
			,mYScale(y_scale)
		{
		}

		osg::Vec3 operator()(const osg::Vec3& v)
		{
			osg::Vec3d vr = osg::Vec3(((int)(mXScale * v[0]))/(float)(mXScale-1.0), ((int)(mYScale * v[1])) / (float)(mYScale-1.0), 0.0);
			vr[0] = osg::clampBetween(vr[0], 0.0, 1.0 - (1.0 / mXScale));
			vr[1] = osg::clampBetween(vr[1], 0.0, 1.0 - (1.0 / mYScale));
			return vr;
		}

		unsigned int mXScale;
		unsigned int mYScale;

	};
	osg::Geometry* geom = (intersection.drawable->asGeometry());
	if (!geom || geom == mSelectionGeometry || !mSelectionGeometry)
	{
		return;
	}

	osgTerrain::HeightFieldLayer* hfl = dynamic_cast<osgTerrain::HeightFieldLayer*>(terrain_tile->getElevationLayer());
	if(hfl)
	{

		osg::Vec3d local;
		osgTerrain::Locator* locator = terrain_tile->getElevationLayer()->getLocator();
		locator->convertModelToLocal(intersection.getWorldIntersectPoint(), local);

		osg::Vec3d offset = osg::Vec3(1.0 / hfl->getNumColumns(), 1.0 / hfl->getNumRows() , 0.0);

		std::vector<osg::Vec3d> v;
		v.resize(10);

		v[0] = local;
		v[1] = local + osg::Vec3d(offset[0],0.0,0.0);
		v[2] = local + osg::Vec3d(offset[0],offset[1],0.0);
		v[3] = local + osg::Vec3d(0.0,offset[1],0.0);
		v[4] = local + osg::Vec3d(-offset[0],offset[1],0.0);
		v[5] = local + osg::Vec3d(-offset[0],0.0,0.0);
		v[6] = local + osg::Vec3d(-offset[0],-offset[1],0.0);
		v[7] = local + osg::Vec3d(0.0,-offset[1],0.0);
		v[8] = local + osg::Vec3d( offset[0],-offset[1],0.0);
		v[9] = local + osg::Vec3d( offset[0],0.0,0.0);
		
		osg::Vec3Array* selected_vertices = dynamic_cast<osg::Vec3Array*>( mSelectionGeometry->getVertexArray() );
		//clamp
		
		
		clampToGrid c2g(hfl->getNumColumns(), hfl->getNumRows());
		for (std::vector<osg::Vec3d>::iterator iter = v.begin(); iter != v.end();++iter)
		{
			(*iter)=c2g(*iter);
			(*iter)[2] = hfl->getHeightField()->getHeight((*iter)[0] * hfl->getNumColumns(), (*iter)[1] * hfl->getNumRows()) + 0.5;
			locator->convertLocalToModel((*iter), (*iter));
			
			(*selected_vertices)[iter - v.begin()] = (*iter);
		}

		selected_vertices->dirty();
		mSelectionGeometry->dirtyBound();
	}

}


