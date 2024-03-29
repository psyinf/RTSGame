#pragma once
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>
#include <osgTerrain/Terrain>

#include <osgGA/GUIEventHandler>
#include "Core/Core.h"
#include "TerrainTechniques.h"




//
namespace nsGameCore{ 

class GameCore;



class Terrain
{
public:
	Terrain(nsGameCore::GameCore& ref_core);
	~Terrain();

	void load(const std::string& base_name);

	osg::ref_ptr<osg::Group> getTerrainNode();

	osgTerrain::TerrainTile* getTerrainTile(float x, float y);

protected:
	osg::Vec2						  mTileSize;
	osg::ref_ptr<osgTerrain::Terrain> mTerrain;
	
	nsGameCore::GameCore&		 mrGameCore;



};
} //namespace