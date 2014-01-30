#include "GameCore.h"
#include <Core/Core.h>
#include "Util/GameTerrain.h"

void nsGameCore::GameCore::frame()
{
	
}

void nsGameCore::GameCore::setup( const std::string& configuration )
{
	//TEST
	mTerrain.reset(new nsGameCore::Terrain(mrCore));
	mTerrain->load("./data/levels/Channelled land.tif" );
	mrCore.getMainRoot()->addChild(mTerrain->getTerrainNode());
	
}

nsGameCore::GameCore::~GameCore()
{

}

nsGameCore::GameCore::GameCore( nsRenderer::Core& render_core ) 
	:mrCore(render_core)
{

}
