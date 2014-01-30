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
