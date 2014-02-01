#include "TerrainTechniques.h"
#include <osgTerrain/Terrain>
#include <osgTerrain/TerrainTile>

bool osgTerrain::ModifyingTerrainTechnique::getHeight( double x, double y, double& height )
{
	if (_terrainTile)
	{
		if (!(x >= 0.0 && x < 1.0 && y >= 0.0 && y < 1.0))
		{
			return false;
		}
		HeightFieldLayer* hfl = dynamic_cast<HeightFieldLayer*>( getTerrainTile()->getElevationLayer() );
		if (hfl)
		{
			osg::HeightField* height_field = hfl->getHeightField();
			if (height_field)
			{
				height = height_field->getHeight(height_field->getNumRows() * x, height_field->getNumColumns() * y);
				return true;
			}
		}
	}
	return false;
}

bool osgTerrain::ModifyingTerrainTechnique::setHeight( double x, double y, double height )
{
	if (_terrainTile)
	{
		if (!(x >= 0.0 && x < 1.0 && y >= 0.0 && y < 1.0))
		{
			return false; 
		}
		HeightFieldLayer* hfl = dynamic_cast<HeightFieldLayer*>( getTerrainTile()->getElevationLayer() );
		if (hfl)
		{

			osg::HeightField* height_field = hfl->getHeightField();
			if (height_field)
			{
				height_field->setHeight(height_field->getNumRows() * x, height_field->getNumColumns() * y, height);
				_terrainTile->setDirtyMask(osgTerrain::TerrainTile::ELEVATION_DIRTY);
				_terrainTile->setDirty(true);
				return true;
			}

		}
	}
	return false;
}

bool osgTerrain::ModifyingTerrainTechnique::setHeight( double x, double y, double radius_ndc, double height )
{
	//TODO´: sample inside the radius and set heights
	return false;
}



void osgTerrain::ModifyingTerrainTechnique::traverse( osg::NodeVisitor& nv )
{
	GeometryTechnique::traverse(nv);
}

void osgTerrain::ModifyingTerrainTechnique::update( osgUtil::UpdateVisitor* nv )
{
	GeometryTechnique::update(nv);
}

void osgTerrain::ModifyingTerrainTechnique::init( int dirtyMask, bool assumeMultiThreaded )
{
	GeometryTechnique::init(dirtyMask, assumeMultiThreaded);
}

osgTerrain::ModifyingTerrainTechnique::~ModifyingTerrainTechnique()
{

}

osgTerrain::ModifyingTerrainTechnique::ModifyingTerrainTechnique( const osgTerrain::GeometryTechnique& technique,const osg::CopyOp& copyop/*=osg::CopyOp::SHALLOW_COPY*/ ) 
	:osgTerrain::GeometryTechnique(technique, copyop)
{

}

osgTerrain::ModifyingTerrainTechnique::ModifyingTerrainTechnique( void ) :osgTerrain::GeometryTechnique()
{

}

bool osgTerrain::ModifyingTerrainTechnique::getExtents(osg::Vec3d& top_left, osg::Vec3d& bottom_right)
{
	if (_terrainTile && _terrainTile->getLocator())
	{
		osgTerrain::Locator* locator = _terrainTile->getLocator();
		locator->convertLocalToModel(osg::Vec3d(0,0,0), top_left);
		locator->convertLocalToModel(osg::Vec3d(1,1,0), bottom_right);
		return true;
	}
	return false;
}

osg::Vec3d osgTerrain::ModifyingTerrainTechnique::getClampedPosition( const osg::Vec3d& pos ) const
{
	if (_terrainTile)
	{
		float x_scale = _terrainTile->getElevationLayer()->getNumColumns();
		float y_scale = _terrainTile->getElevationLayer()->getNumRows();
		osg::Vec3d vr = osg::Vec3(((int)(x_scale * pos[0]))/(float)(x_scale-1.0), ((int)(y_scale * pos[1])) / (float)(y_scale-1.0), pos[2]);
		vr[0] = osg::clampBetween(vr[0], 0.0, 1.0 - (1.0 / x_scale));
		vr[1] = osg::clampBetween(vr[1], 0.0, 1.0 - (1.0 / y_scale));
		return vr;
	}
	return osg::Vec3d();
	
}

osg::Vec3i osgTerrain::ModifyingTerrainTechnique::getTilePosition( const osg::Vec3d& clamped_normalized ) const
{
	osg::Vec3d world_pos;
	if (_terrainTile)
	{
		
		osg::Vec3d clamped  = this->getClampedPosition(clamped_normalized);
		osgTerrain::Locator* locator = _terrainTile->getElevationLayer()->getLocator();
		locator->convertLocalToModel(clamped, world_pos);
	}
	return osg::Vec3i(world_pos[0], world_pos[1], world_pos[2]);
}
