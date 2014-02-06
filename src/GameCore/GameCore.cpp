#include "GameCore.h"
#include "GameArea.h"

#include <Core/Core.h>

#include "Util/GameTerrain.h"
#include "Util/GameModels.h"
#include "Util/HUDManager.h"

#include <boost/algorithm/string.hpp>
#include <Core/StandardHandlers.h>
#include <Common/CommonHelpers.h>

#include <osg/PositionAttitudeTransform>

nsGameCore::GameCore::GameCore( nsRenderer::Core& render_core ) 
	:mrCore(render_core)
	,mTextNode(new osg::Geode)
{

}

nsRenderer::Core& nsGameCore::GameCore::getRenderCore()
{
	return mrCore;
}

void nsGameCore::GameCore::frame()
{
	
}

void nsGameCore::GameCore::setup( const std::string& configuration )
{
	//TEST
	mTerrain.reset(new nsGameCore::Terrain(*this));
	mTerrain->load("./data/levels/Channelled land.tif" );
	mrCore.getMainRoot()->addChild(mTerrain->getTerrainNode());
	
	mModelManager.reset(new nsGameCore::GameModelManager("./data/models"));
	mGameArea.reset(new nsGameCore::GameArea(*this));
	


	mCurrentEditMode.addRegisteredMode("DEFAULT");
	mCurrentEditMode.addRegisteredMode("TERRAIN_UP");
	mCurrentEditMode.addRegisteredMode("TERRAIN_DOWN");
	mCurrentEditMode.addRegisteredMode("PLACE");
	mCurrentEditMode.addRegisteredMode("LEVEL_TERRAIN");
	
	std::vector<std::string> model_names;
	mModelManager->getRegisteredModelNames(model_names);
	for (auto iter = model_names.begin(); iter != model_names.end(); ++iter)
	{
		mCurrentEditMode.addSubMode("PLACE", *iter);
	}

	mCurrentEditMode.setMode("DEFAULT");
	//Add top level HUD
	//mHUDCamera = nsRenderer::Helpers::createHUDCamera(1024,1024);
	
	mrCore.getMainRoot()->addChild(mHUDCamera);
	nsRenderer::CamResizeHandler* cam_resize_handler = new nsRenderer::CamResizeHandler(mHUDCamera);
	mrCore.addEventHandler(cam_resize_handler);
	//mHUDCamera->addChild(mTextNode);

	mHUDManager = boost::shared_ptr<HUDManager>(new HUDManager(*this));
}

nsGameCore::GameCore::~GameCore()
{

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

void nsGameCore::GameCore::placeModel( osg::Vec3d& position, osg::Quat& orientation, osg::Vec3 scale,  const std::string& model_type )
{
	try 
	{
		boost::shared_ptr<nsGameCore::GameModel> game_model = mModelManager->createGameModelInstance(model_type);
		if (!game_model)
		{
			//TODO: bark here
			return;
		}
		osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
		pat->setPosition(position);
		pat->setAttitude(orientation);
		pat->setScale(scale);
		pat->addChild(game_model->getGraphicalModel());
		pat->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON); 
		mrCore.getSubRoot("MODEL_ROOT")->addChild(pat);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
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

void nsGameCore::GameCore::setCellData( const CellAdress& address, CellDataPtr cell_data_ptr )
{
	mGameArea->setCellData(address, cell_data_ptr);
}

nsGameCore::GameModelManager& nsGameCore::GameCore::getModelManager()
{
	return *mModelManager;
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

