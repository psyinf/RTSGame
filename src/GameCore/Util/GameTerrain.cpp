#include "GameTerrain.h"
#include <GameCore/GameCore.h>
#include <RenderCore/RenderCore.h>
#include <GameCore/Util/TerrainPicker.h>
#include <osgDB/FileNameUtils>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/Material>
#include <osgTerrain/DisplacementMappingTechnique>

#include "Util/Config.h"
#include <nlohmann/json.hpp>
namespace config
{
struct TerrainDescriptor
{
    std::string diffuse;
    std::string height;
    float       height_scale = 1.0;
};


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TerrainDescriptor, diffuse, height, height_scale)
}

 // namespace config
osg::ref_ptr<osg::Group> nsGameCore::Terrain::getTerrainNode()
{
	return mTerrain;
}

void nsGameCore::Terrain::load( const std::string& descriptor )
{
    config::TerrainDescriptor            desc              = config::load<config::TerrainDescriptor>(descriptor);
    /*
	const std::string height_field_path = osgDB::getNameLessExtension(base_name) + "_height" + osgDB::getFileExtensionIncludingDot(base_name) +".gdal";
	const std::string diff_field_path = osgDB::getNameLessExtension(base_name) + "_diff" + osgDB::getFileExtensionIncludingDot(base_name) +".gdal";
	*/
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
	options->setBuildKdTreesHint(osgDB::Options::BUILD_KDTREES);


	osg::HeightField*                                   height_field               = osgDB::readHeightFieldFile(desc.height + ".gdal", options);
	osg::ref_ptr<osgTerrain::ModifyingTerrainTechnique> terrain_geometry_technique = new osgTerrain::ModifyingTerrainTechnique();
	terrain_geometry_technique->setFilterMatrixAs(osgTerrain::GeometryTechnique::GAUSSIAN);
	
	if (!height_field)
	{
		throw (std::runtime_error("Cannot load specified height field: " + desc.height));
	}
	
	//try to find a visual layer
	//create a locator to move/scale the data
	osg::ref_ptr<osgTerrain::Locator> Locator1 = new osgTerrain::Locator;
	Locator1->setCoordinateSystemType( osgTerrain::Locator::PROJECTED );
	
	Locator1->setTransformAsExtents(0.0, 0.0, height_field->getNumColumns(), height_field->getNumRows());

	//create a height field layer from the height field, using the locator
	osg::Texture::FilterMode filter = osg::Texture::LINEAR;
	
	osg::ref_ptr<osgTerrain::HeightFieldLayer> hf_layer = new osgTerrain::HeightFieldLayer(height_field);
	hf_layer->setLocator(Locator1);
	hf_layer->setMagFilter(filter);
	hf_layer->transform(0,desc.height_scale);
	//create a terrain-tile and add the height field
	osg::ref_ptr<osgTerrain::TerrainTile> terrain_tile = new osgTerrain::TerrainTile;
	terrain_tile->setElevationLayer( hf_layer.get() );
	terrain_tile->setRequiresNormals(true);
	

	terrain_tile->setTerrainTechnique(terrain_geometry_technique);
	terrain_tile->setDirty(true);
	
	osg::Image* diffuse_texture = osgDB::readImageFile(desc.diffuse +".gdal");
	if (diffuse_texture)
	{
		terrain_tile->setColorLayer(0, new osgTerrain::ImageLayer(diffuse_texture));
	}

	mTerrain->addChild( terrain_tile.get() );

	mTerrain->setTerrainTechniquePrototype(terrain_geometry_technique);
	
	osg::ref_ptr<osgGA::EventHandler> pick_handler = new nsGameCore::PickHandler(mrGameCore);
	
	mrGameCore.getRenderCore().getViewer()->addEventHandler(pick_handler);
}

nsGameCore::Terrain::~Terrain()
{

}

nsGameCore::Terrain::Terrain( nsGameCore::GameCore& ref_core ) 
	:mTerrain( new osgTerrain::Terrain)
	,mrGameCore(ref_core)
	,mTileSize(2048,2048)
{

}

osgTerrain::TerrainTile* nsGameCore::Terrain::getTerrainTile( float x, float y )
{
	//childs are added row-wise (x)
	//for now we only have one tile, so normalized coordinates are supposed to be between 0..1
	if (x < 1.0 && x >= 0.0 && y < 1.0 && y >= 0.0 && mTerrain->getNumChildren() > 0)
	{
		osgTerrain::TerrainTile* terrain_tile = dynamic_cast<osgTerrain::TerrainTile*>(mTerrain->getChild(0));
		return terrain_tile;
	}
	return 0;
}

