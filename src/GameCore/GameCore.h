#pragma once

#include <boost/shared_ptr.hpp>
#include <string>


namespace nsRenderer
{
	class Core;
}
namespace nsGameCore{

class Terrain;

class GameCore
{
public:
	GameCore(nsRenderer::Core& render_core);
	~GameCore();

	void setup(const std::string& configuration);

	void frame();
protected:
	nsRenderer::Core& mrCore;
	boost::shared_ptr<nsGameCore::Terrain> mTerrain;

	
};


} //nsGameCore