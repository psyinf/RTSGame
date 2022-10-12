#include "TerrainTechniques.h"
#include <osgTerrain/Terrain>
#include <osgTerrain/TerrainTile>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/RayIntersector>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <iostream>


osg::Vec3d sampleUniform(float u1, float u2)
{
/* Later
	const float r = gmtl::Math::sqrt(1.0f - u1 * u1);
	const float phi = 2 * osg::PI * u2;
	osg::Vec3d result = osg::Vec3d(gmtl::Math::cos(phi) * r, gmtl::Math::sin(phi) * r, u1);	
	*/
	osg::Vec3d result = osg::Vec3d(0,0,1);
	return result;

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


void osgTerrain::ModifyingTerrainTechnique::getVertices( double x, double y, std::vector<osg::Vec3>& vertices ) const
{
	if (_terrainTile)
	{
		if (!(x >= 0.0 && x < 1.0 && y >= 0.0 && y < 1.0))
		{
			return;
		}
		const HeightFieldLayer* hfl = dynamic_cast<const HeightFieldLayer*>( getTerrainTile()->getElevationLayer() );
		if (hfl)
		{
			const osg::HeightField* height_field = hfl->getHeightField();
			if (height_field)
			{
				//height = height_field->getHeight(height_field->getNumRows() * x, height_field->getNumColumns() * y);
				// 				return true;
				double delta_x = 1.0 / height_field->getNumRows();
				double delta_y = 1.0 / height_field->getNumColumns();
				//0/0
				vertices.push_back(height_field->getVertex(height_field->getNumRows() * x, height_field->getNumColumns() * y));
				//-/-
				vertices.push_back(height_field->getVertex(height_field->getNumRows() * osg::clampBetween(x - 1.0 * delta_x, 0.0, 1.0 - delta_x) , height_field->getNumColumns() * osg::clampBetween(y - 1.0 * delta_y, 0.0, 1.0 -delta_y)));
				//+/-
				vertices.push_back(height_field->getVertex(height_field->getNumRows() * osg::clampBetween(x + 1.0 * delta_x, 0.0, 1.0 - delta_x) , height_field->getNumColumns() * osg::clampBetween(y - 1.0 * delta_y, 0.0, 1.0 -delta_y)));
				//+/+
				vertices.push_back(height_field->getVertex(height_field->getNumRows() * osg::clampBetween(x + 1.0 * delta_x, 0.0, 1.0 - delta_x) , height_field->getNumColumns() * osg::clampBetween(y + 1.0 * delta_y, 0.0, 1.0 -delta_y)));
				//-/+
				vertices.push_back(height_field->getVertex(height_field->getNumRows() * osg::clampBetween(x - 1.0 * delta_x, 0.0, 1.0 - delta_x) , height_field->getNumColumns() * osg::clampBetween(y + 1.0 * delta_y, 0.0, 1.0 -delta_y)));
				//0/+
				vertices.push_back(height_field->getVertex(height_field->getNumRows() * osg::clampBetween(x, 0.0, 1.0 - delta_x) , height_field->getNumColumns() * osg::clampBetween(y + 1.0 * delta_y, 0.0, 1.0 -delta_y)));
				//0/-
				vertices.push_back(height_field->getVertex(height_field->getNumRows() * osg::clampBetween(x, 0.0, 1.0 - delta_x) , height_field->getNumColumns() * osg::clampBetween(y - 1.0 * delta_y, 0.0, 1.0 -delta_y)));
				//-/0
				vertices.push_back(height_field->getVertex(height_field->getNumRows() * osg::clampBetween(x - 1.0 * delta_x, 0.0, 1.0 - delta_x) , height_field->getNumColumns() * osg::clampBetween(y, 0.0, 1.0 -delta_y)));
				//+/0
				vertices.push_back(height_field->getVertex(height_field->getNumRows() * osg::clampBetween(x + 1.0 * delta_x, 0.0, 1.0 - delta_x) , height_field->getNumColumns() * osg::clampBetween(y, 0.0, 1.0 -delta_y)));
			}

		}
	}
	
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
		osgTerrain::Locator const* locator = _terrainTile->getLocator();
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
	/* Later
	HeightFieldLayer* hfl = dynamic_cast<HeightFieldLayer*>( getTerrainTile()->getElevationLayer() );
	if (hfl)
	{
		//try to load ambient apperture
		osg::ref_ptr<osg::Image> ambient = osgDB::readImageFile("d:/test.tif");
		
		if (!ambient)
		{
			osgTerrain::Locator* locator = hfl->getLocator();
			double step_row = 1.0 / hfl->getNumRows();
			double step_col = 1.0 / hfl->getNumColumns();

			//	
			std::vector<unsigned char> image_data;
			//http://www.cse.ust.hk/~psander/docs/aperture.pdf

			//osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(osg::Vec3d(), osg::Vec3d());
			osg::ref_ptr<osgUtil::RayIntersector> lsi = new osgUtil::RayIntersector();
			
			lsi->setCoordinateFrame(osgUtil::Intersector::MODEL);
			lsi->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
			lsi->setPrecisionHint(osgUtil::Intersector::USE_DOUBLE_CALCULATIONS);
			for (double row_idx = 0.0; row_idx < 1.0; row_idx+=step_row)
			{
				std::cout << "Processing ambient occlusion data for row " << row_idx * hfl->getNumRows() << "/" <<  hfl->getNumRows() << std::endl;
				for (double col_idx = 0.0; col_idx < 1.0; col_idx+=step_col)
				{
					osg::Vec3d result_normal(0,0,0);
					float apperture = 0.0;
					osg::Vec3d model_coords;
					osg::Vec3d local_coords(row_idx, col_idx  ,0.5);
					
					
			
				
					locator->convertLocalToModel(local_coords, model_coords);
					
					//getHeight(local_coords[0], local_coords[1],model_coords[2]);
					
					std::vector<osg::Vec3> vertices;
					getVertices(row_idx, col_idx, vertices);
					for (auto iter = vertices.begin(); iter != vertices.end(); ++iter)
					{
						model_coords[2]= std::max<double>(model_coords[2], (*iter)[2]);
					}
					model_coords[2] += 0.01;
					//sample 
					unsigned int num_samples = 16;		
					for (unsigned int i = 0; i < num_samples; ++i)
					{
						double u1 = gmtl::Math::rangeRandom(0.0,1.0);
						double u2 = gmtl::Math::rangeRandom(0.0,1.0);
						osg::Vec3d rand_vector = sampleUniform(u1, u2);
						//get ray intersection
						lsi->reset();
						lsi->setStart(model_coords);
						lsi->setDirection(rand_vector);
						osgUtil::IntersectionVisitor isv(lsi);
						isv.setUseKdTreeWhenAvailable(true);
						
						getTerrainTile()->accept(isv);

						if (!lsi->containsIntersections())
						{
							result_normal += rand_vector * (1.0 / num_samples);
							apperture+= (1.0 / num_samples);

							//std::cout << "sdf" << std::endl;
						}
						else
						{
							//TODO:check: this is intersecting point immediately above, which is stupip
							//might be, that we have to consider slope at the point
						//	osg::Vec3d intersection_point = lsi->getFirstIntersection().getWorldIntersectPoint();
						//	osg::Vec3d intersection_normal= lsi->getFirstIntersection().getWorldIntersectNormal();
						//	osg::Drawable* drawable =  lsi->getFirstIntersection().drawable;
						//	osg::Array* vertex_array =drawable->asGeometry()->getVertexArray();
						//	osg::Vec3Array* vert_array = dynamic_cast<osg::Vec3Array*>(vertex_array);

						//	for (unsigned int i = 0; i < lsi->getFirstIntersection().indexList.size(); ++i)
						//	{
						//		osg::Vec3f vertex = (*vert_array)[lsi->getFirstIntersection().indexList[i]];
							//	std::cout << i;
						//	}
							//std::cout << "blocked by " << std::endl;
							
						}
					}
					//get the "visibility" term
					apperture = gmtl::Math::aCos(1.0 - apperture ) / gmtl::Math::PI_OVER_2;
					if (apperture > gmtl::Math::PI_OVER_2)
					{
						std::cout << "big app: "<<apperture << std::endl;
					}
					unsigned char app_int = 255 * apperture;
					image_data.push_back(static_cast<char>(result_normal[0] * 255));
					image_data.push_back(static_cast<char>(result_normal[1] * 255));
					image_data.push_back(static_cast<char>(result_normal[2] * 255));
					image_data.push_back(app_int);

					

				}
			}
			ambient = new osg::Image;
			ambient->setImage(hfl->getNumColumns(),hfl->getNumRows(), 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,(unsigned char*)(&image_data[0]), osg::Image::NO_DELETE);
			osgDB::writeImageFile(*ambient, "d:/test.tif");	
		}
		ambient->setOrigin(osg::Image::TOP_LEFT);
		getTerrainTile()->setColorLayer(getTerrainTile()->getNumColorLayers(), new osgTerrain::ImageLayer(ambient));
		osg::Uniform* sampler = new osg::Uniform("ambient", 1);
		getTerrainTile()->getOrCreateStateSet()->addUniform(sampler);

	}*/
}

void osgTerrain::ModifyingTerrainTechnique::generateGeometry( osgTerrain::GeometryTechnique::BufferData& buffer,osgTerrain::Locator* masterLocator, const osg::Vec3d& centerModel )
{
	osgTerrain::GeometryTechnique::generateGeometry(buffer, masterLocator, centerModel);
	//calculateAmbientApperture();
}

