#include "GameCore.h"
#include "GameArea.h"
#include "RenderCore/RenderCore.h"
#include "RenderCore/CamResizeHandler.h"

#include "Util/GameTerrain.h"
#include "Util/GameModels.h"
#include "Util/HUDManager.h"

#include "GameLogic.h"


#include <boost/algorithm/string.hpp>

#include <functional>
//#include <Core/StandardHandlers.h>

#include <osg/PositionAttitudeTransform>

struct SetConfig
{
    std::string name      = "default.yaml";
   
};
nsGameCore::GameCore::GameCore(  renderer::RenderCore& render_core ) 
	:mrRenderCore(render_core)

{

}

void nsGameCore::GameCore::frame()
{
	if (mGameLogic)
	{
		mGameLogic->frame();	
	}
}

void nsGameCore::GameCore::setup( const std::string& configuration )
{
	//Todo: from configuration
    SetConfig setConf;
    
	mTerrain.reset(new nsGameCore::Terrain(*this));
	mTerrain->getTerrainNode()->setNodeMask(0x0000ffff);
    //#TODO: filesystem paths
    mTerrain->load("./data/levels/" + setConf.name);

    mrRenderCore.getMainRoot()->addChild(mTerrain->getTerrainNode());
    mrRenderCore.getMainRoot()->addChild(mrRenderCore.getSubRoot("MAIN_ROOT"));
	mModelManager.reset(new nsGameCore::GameModelManager("./data/models"));
	mGameArea.reset(new nsGameCore::GameArea(*this));
	


	mCurrentEditMode.addRegisteredMode("DEFAULT");
	mCurrentEditMode.addRegisteredMode("PLACE");
	mCurrentEditMode.addRegisteredMode("TERRAIN");
	
	std::vector<std::string> model_names;
	mModelManager->getRegisteredModelNames(model_names);
	for (const auto& model : model_names)
	{
		mCurrentEditMode.addSubMode("PLACE", model);
	}

	mCurrentEditMode.setMode("DEFAULT");
	
	
	mrRenderCore.getMainRoot()->addChild(mHUDCamera);
    mrRenderCore.getViewer()->addEventHandler(new renderer::CamResizeHandler(mHUDCamera));
	
	mHUDManager = std::make_shared<nsGameCore::HUDManager>(*this);
	//
	std::vector<std::string> buildings;
	mModelManager->getRegisteredModelNames(buildings);
	mHUDManager->addMenu("Buildings");
	mHUDManager->addMenu("Terrain");
	mHUDManager->addMenu("Debug");
	for (auto building : buildings)
	{	
		mHUDManager->getMenu("Buildings")->addEntry(building, [this, &building]() { this->setModeAndEditMode("Place", building); });
	}
	
	//mHUDManager->getMenu("Terrain")->addEntry("Raise", std::function<void()>(std::bind(&GameCore::setModeAndEditMode, this, "Terrain", "Up")));
	mHUDManager->getMenu("Terrain")->addEntry("Raise", [this]() {this->setModeAndEditMode("Terrain", "Up"); });
    mHUDManager->getMenu("Terrain")->addEntry("Lower", [this]() { this->setModeAndEditMode("Terrain", "Down"); });
    mHUDManager->getMenu("Terrain")->addEntry("Level", [this]() { this->setModeAndEditMode("Terrain", "Level"); });
	mGameLogic.reset(new GameLogic(*this));

}




nsGameCore::EditMode& nsGameCore::GameCore::getCurrentEditMode()
{
	return mCurrentEditMode;
}

const nsGameCore::EditMode& nsGameCore::GameCore::getCurrentEditMode() const
{
	return mCurrentEditMode;
}
void nsGameCore::GameCore::setCurrentEditMode( const nsGameCore::EditMode& edit_mode )
{
	mCurrentEditMode = edit_mode;
}

osg::Camera* nsGameCore::GameCore::getHUDCamera()
{
	return mHUDCamera;
}

osgText::Text* nsGameCore::GameCore::getNamedTextObject( const std::string& text_elem_name)
{
	auto iter = (mHUDTextElements.find(text_elem_name));
	if (iter != mHUDTextElements.end())
	{
		return (*iter).second;
	}
	else
	{
		return 0;
	}

}

void nsGameCore::GameCore::createNamedTextObject( const std::string& text_elem_name , osg::ref_ptr<osgText::Text> text_node )
{
	mHUDTextElements[text_elem_name] = text_node;
	text_node->setDataVariance(osg::Object::DYNAMIC);
	
	mTextNode->addDrawable(text_node);
	
}


void nsGameCore::GameCore::placeModel( const CellAdress& address, const std::string& model_type, float height )
{
	std::shared_ptr<nsGameCore::GameModel> game_model = mModelManager->createGameModelInstance(model_type);
	if (!game_model || !game_model->getGraphicalModel())
	{
		//TODO: bark here
		return;
	}
	osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
	//get local offset
	pat->setPosition(osg::Vec3d(address.x(),address.y(),address.z() + height) + game_model->getPlacementMatrix().getTrans());
	pat->setAttitude(game_model->getPlacementMatrix().getRotate());
	pat->setScale(game_model->getModelScale());
	pat->addChild(game_model->getGraphicalModel());
	pat->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON); 
	mrRenderCore.getSubRoot("MODEL_ROOT")->addChild(pat);
	
	auto x = std::make_shared<nsGameCore::CellData>(address, game_model);
	 setCellData(address, x);
}



nsGameCore::CellAdress nsGameCore::GameCore::calculateCellAdress( float x, float y, unsigned int level /*= 0*/ )
{
	osgTerrain::TerrainTile* terrain_tile = mTerrain->getTerrainTile(x,y);
	if (terrain_tile)
	{
		osgTerrain::ModifyingTerrainTechnique* mod_technique = dynamic_cast<osgTerrain::ModifyingTerrainTechnique*>(terrain_tile->getTerrainTechnique());
		if (mod_technique)
		{
			return nsGameCore::CellAdress(mod_technique->getTilePosition(osg::Vec3(x,y,0)));
		}
	}
	return nsGameCore::CellAdress();

}

nsGameCore::CellDataPtr nsGameCore::GameCore::getCellData( const CellAdress& address )
{
	return mGameArea->getCellData(address);
}

void nsGameCore::GameCore::setCellData( const CellAdress& address, CellDataPtr cell_data_ptr ) const
{
	mGameArea->setCellData(address, cell_data_ptr);
}

renderer::RenderCore& nsGameCore::GameCore::getRenderCore()
{
    return mrRenderCore;
}

nsGameCore::GameModelManager& nsGameCore::GameCore::getModelManager()
{
	return *mModelManager;
}

void nsGameCore::GameCore::setModeAndEditMode( const std::string& mode, const std::string& sub_mode )
{
	//TODO: log
    std::cout << "Mode " << mode << " Edit mode: " << sub_mode << std::endl;
	mCurrentEditMode.setMode(mode);
	mCurrentEditMode.setSubMode(sub_mode);
}

std::string nsGameCore::EditMode::getCurrentModeName() const
{
	return mModeName;
}

bool nsGameCore::EditMode::setMode( const std::string& new_mode_name )
{
	if (isRegisteredMode(new_mode_name))
	{
		mModeName = boost::to_upper_copy(new_mode_name);
		return true;
	}
	return false;
}

std::string nsGameCore::EditMode::nextMode()
{
	auto iter = std::find(mAvailableModes.begin(), mAvailableModes.end(), mModeName);
	if (iter != mAvailableModes.end())
	{
		++iter;
		if (iter == mAvailableModes.end())
		{
			iter = mAvailableModes.begin();
		}
		mModeName = (*iter);
	}
	else
	{
		mModeName = *mAvailableModes.begin();
	}
	return mModeName;
}

bool nsGameCore::EditMode::isRegisteredMode( const std::string& mode_name ) const
{
	return (mAvailableModes.end() != std::find(mAvailableModes.begin(), mAvailableModes.end(), boost::to_upper_copy(mode_name)));
}

void nsGameCore::EditMode::addRegisteredMode( const std::string& mode_name )
{
	if (!isRegisteredMode(mode_name))
	{
		mAvailableModes.push_back(boost::to_upper_copy(mode_name));
		mSubModes.insert(std::make_pair(mode_name, std::vector<std::string>()));
	}
}

nsGameCore::EditMode::EditMode() 
	:mModeName()
	,mSubModeName()
{

}

void nsGameCore::EditMode::addSubMode( const std::string& mode_name, const std::string& sub_mode_name )
{
	if (isRegisteredMode(mode_name))
	{
		auto find_iter = mSubModes.find(boost::to_upper_copy(mode_name));
		assert(find_iter != mSubModes.end());

		std::vector<std::string>& sub_modes = (*find_iter).second;
		if (sub_modes.end() == std::find(sub_modes.begin(), sub_modes.end(), sub_mode_name) )
		{
			sub_modes.push_back(boost::to_upper_copy(sub_mode_name));
		}
	}
}

bool nsGameCore::EditMode::hasSubMode( const std::string& mode_name, const std::string& sub_mode_name ) const
{
	if (isRegisteredMode(mode_name))
	{
		return (0 != mSubModes.count(sub_mode_name));
	}
	return false;
}

std::string nsGameCore::EditMode::getSubMode() const
{
	return mSubModeName;
}

std::string nsGameCore::EditMode::nextSubMode()
{
	auto iter = mSubModes.find(mModeName);
	if (iter != mSubModes.end())
	{
		std::vector<std::string>& sub_modes = (*iter).second;
		auto sub_iter = std::find(sub_modes.begin(), sub_modes.end(), mSubModeName);
		if (sub_modes.empty())
		{
			mSubModeName = "";
		}
		else if (sub_iter == sub_modes.end())
		{
			mSubModeName = (*sub_modes.begin());
		}
		else
		{
			++sub_iter;
			if (sub_iter == sub_modes.end())
			{
				sub_iter = sub_modes.begin();
			}
			mSubModeName = (*sub_iter);
		}
		return mSubModeName;
	}
	return "";

}

void nsGameCore::EditMode::setSubMode( const std::string& sub_mode_name )
{
	if (hasSubMode(mModeName, sub_mode_name))
	{
		throw std::runtime_error("Invalid sub mode: " + sub_mode_name );
	}
	mSubModeName = sub_mode_name;
}

nsGameCore::NamedValue& nsGameCore::GameCore::getNamedValue( const std::string& name )
{
	if (mNamedValues.count(name))
	{
		return mNamedValues[name];
	}
	throw (std::runtime_error("Unknown Named Value: " + name));
}

nsGameCore::NamedValue& nsGameCore::GameCore::addNamedValue( const std::string& name, const nsGameCore::NamedValue::ValueType& value_type )
{
	if (!mNamedValues.count(name))
	{
		mNamedValues[name] = NamedValue(name, value_type);
		return mNamedValues[name];
	}
	throw std::runtime_error("Unregistered NamedValue:" + name);
}

std::shared_ptr<nsGameCore::HUDManager> nsGameCore::GameCore::getHUDManager()
{
	return mHUDManager;
}

std::shared_ptr<nsGameCore::GameArea> nsGameCore::GameCore::getGameArea()
{
	return mGameArea;
}

nsGameCore::Properties& nsGameCore::GameCore::getProperties()
{
	return mProperties;
}

