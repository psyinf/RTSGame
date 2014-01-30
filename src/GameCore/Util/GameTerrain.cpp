#include "GameTerrain.h"
#include <osgDB/FileNameUtils>



osg::ref_ptr<osg::Group> nsGameCore::Terrain::getTerrainNode()
{
	return mTerrain;
}

void nsGameCore::Terrain::load( const std::string& base_name )
{
	//osgTerrain::Terrain* terrain = new osgTerrain::Terrain;
	const std::string height_field_path = osgDB::getNameLessExtension(base_name) + "_height" + osgDB::getFileExtensionIncludingDot(base_name) +".gdal";
	const std::string diff_field_path = osgDB::getNameLessExtension(base_name) + "_diff" + osgDB::getFileExtensionIncludingDot(base_name) +".gdal";
	osg::HeightField* height_field = osgDB::readHeightFieldFile(height_field_path);
	//height_field->setXInterval(16);
	//height_field->setYInterval(16);
	osg::ref_ptr<osgTerrain::GeometryTechnique> terrain_geometry_technique = new osgTerrain::ModifyingTerrainTechnique();
	terrain_geometry_technique->setFilterMatrixAs(osgTerrain::GeometryTechnique::GAUSSIAN);
	

	if (!height_field)
	{
		throw (std::runtime_error("Cannot load specified height field: " + base_name));
	}
	//try to find a visual layer
	

	//create a locator to move/scale the data
	osg::ref_ptr<osgTerrain::Locator> Locator1 = new osgTerrain::Locator;
	Locator1->setCoordinateSystemType( osgTerrain::Locator::PROJECTED );
	Locator1->setTransformAsExtents( 0.0, 0.0, 2048, 2048 );

	//create a height field layer from the height field, using the locator
	osg::Texture::FilterMode filter = osg::Texture::LINEAR;
	osg::ref_ptr<osgTerrain::HeightFieldLayer> hf_layer = new osgTerrain::HeightFieldLayer(height_field);
	hf_layer->setLocator(Locator1);
	hf_layer->setMagFilter(filter);

	//create a terrain-tile and add the height field
	osg::ref_ptr<osgTerrain::TerrainTile> terrain_tile = new osgTerrain::TerrainTile;
	terrain_tile->setElevationLayer( hf_layer.get() );
	terrain_tile->setRequiresNormals(true);

	terrain_tile->setTerrainTechnique(terrain_geometry_technique);
	terrain_tile->setDirty(true);
	osg::Image* diffuse_texture = osgDB::readImageFile(diff_field_path);
	if (diffuse_texture)
	{
		terrain_tile->setColorLayer(0, new osgTerrain::ImageLayer(diffuse_texture));
	}

	mTerrain->addChild( terrain_tile.get() );
	mTerrain->setTerrainTechniquePrototype(terrain_geometry_technique);
	osg::ref_ptr<osgGA::EventHandler> filter_handler = new FilterHandler(terrain_geometry_technique);
	osg::ref_ptr<osgGA::EventHandler> pick_handler = new PickHandler();
	mrCore.addEventHandler(filter_handler);
	mrCore.addEventHandler(pick_handler);
}

nsGameCore::Terrain::~Terrain()
{

}

nsGameCore::Terrain::Terrain( nsRenderer::Core& ref_core ) 
	:mTerrain( new osgTerrain::Terrain)
	,mrCore(ref_core)
{

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
