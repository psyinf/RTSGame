#include "TerrainTechniques.h"
#include <osgTerrain/Terrain>
#include <osgTerrain/TerrainTile>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/RayIntersector>
#include <osgDB/WriteFile>
#include <gmtl/gmtl.h>
#include <iostream>


osg::Vec3d sampleUniform(float u1, float u2)
{
	const float r = gmtl::Math::sqrt(1.0f - u1 * u1);
	const float phi = 2 * osg::PI * u2;
	return osg::Vec3d(gmtl::Math::cos(phi) * r, gmtl::Math::sin(phi) * r, u1);	
}


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
	//calculateAmbientApperture();
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

void osgTerrain::ModifyingTerrainTechnique::calculateAmbientApperture()
{
	
	HeightFieldLayer* hfl = dynamic_cast<HeightFieldLayer*>( getTerrainTile()->getElevationLayer() );
	if (hfl)
	{
		osg::Vec4ubArray* data_array = new osg::Vec4ubArray(hfl->getNumRows() * hfl->getNumColumns() * 4);
		osgTerrain::Locator* locator = hfl->getLocator();
		double step_row = 1.0 / hfl->getNumRows();
		double step_col = 1.0 / hfl->getNumColumns();

		osg::Image* ambient = new osg::Image;
		ambient->setImage(hfl->getNumRows(), hfl->getNumColumns(), 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,(unsigned char*)(&data_array[0]), osg::Image::USE_NEW_DELETE);
		//http://www.cse.ust.hk/~psander/docs/aperture.pdf
		for (double row_idx = 0.0; row_idx < 1.0; row_idx+=step_row)
		{
			for (double col_idx = 0.0; col_idx < 1.0; col_idx+=step_col)
			{
				osg::Vec3d result_normal;
				float apperture = 0.0;
				osg::Vec3d model_coords;
				osg::Vec3d local_coords(row_idx, col_idx,0);
				locator->convertLocalToModel(local_coords, model_coords);
				getHeight(local_coords[0], local_coords[1],model_coords[2]);
				//sample 
				for (unsigned int i = 0; i < 64; ++i)
				{
					double u1 = gmtl::Math::rangeRandom(0.0,1.0);
					double u2 = gmtl::Math::rangeRandom(0.0,1.0);
					osg::Vec3d rand_vector = sampleUniform(u1, u2);
					//get ray intersection
					osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(model_coords, model_coords + rand_vector * 1000.0 );
					osgUtil::IntersectionVisitor isv(lsi);
					getTerrainTile()->accept(isv);
					
					if (lsi->containsIntersections())
					{
						result_normal += rand_vector * (1.0 / 64.0);
						apperture+= (1.0 / 64.0);
						
						//std::cout << "sdf" << std::endl;
					}
				}
				//get the "visibility" term
				apperture = gmtl::Math::aCos(1.0 - apperture );
				int app_int = 255 * apperture;
				memcpy(ambient->data(hfl->getNumRows() * row_idx, hfl->getNumColumns() * col_idx), &app_int, 1);

				std::cout << apperture << std::endl;
				
			}
		}
		osgDB::writeImageFile(*ambient, "d:/test.jpg");	
	}
}

void osgTerrain::ModifyingTerrainTechnique::generateGeometry( osgTerrain::GeometryTechnique::BufferData& buffer,osgTerrain::Locator* masterLocator, const osg::Vec3d& centerModel )
{
	osgTerrain::GeometryTechnique::generateGeometry(buffer, masterLocator, centerModel);
	//calculateAmbientApperture();
}

