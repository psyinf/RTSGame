#include "GameCore.h"
#include <Core/Core.h>
#include "Util/GameTerrain.h"
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
	

	mCurrentEditMode.addRegisteredMode("DEFAULT");
	mCurrentEditMode.addRegisteredMode("TERRAIN_UP");
	mCurrentEditMode.addRegisteredMode("TERRAIN_DOWN");
	mCurrentEditMode.addRegisteredMode("PLACE");
	mCurrentEditMode.addRegisteredMode("LEVEL_TERRAIN");
	mCurrentEditMode.setMode("DEFAULT");
	//Add top level HUD
	mHUDCamera = nsRenderer::Helpers::createHUDCamera(1024,1024);
	
	mrCore.getMainRoot()->addChild(mHUDCamera);
	nsRenderer::CamResizeHandler* cam_resize_handler = new nsRenderer::CamResizeHandler(mHUDCamera);
	mrCore.addEventHandler(cam_resize_handler);
	mHUDCamera->addChild(mTextNode);
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
	osg::Node* model = osgDB::readNodeFile(model_type);
	osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
	pat->setPosition(position);
	pat->setAttitude(orientation);
	pat->setScale(scale);
	pat->addChild(model);
	mrCore.getSubRoot("MODEL_ROOT")->addChild(pat);

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
	auto iter = std::find(mAvaiableModes.begin(), mAvaiableModes.end(), mModeName);
	if (iter != mAvaiableModes.end())
	{
		++iter;
		if (iter == mAvaiableModes.end())
		{
			iter = mAvaiableModes.begin();
		}
		mModeName = (*iter);
	}
	else
	{
		mModeName = *mAvaiableModes.begin();
	}
	return mModeName;
}

bool nsGameCore::EditMode::isRegisteredMode( const std::string& mode_name )
{
	return (mAvaiableModes.end() != std::find(mAvaiableModes.begin(), mAvaiableModes.end(), boost::to_upper_copy(mode_name)));
}

void nsGameCore::EditMode::addRegisteredMode( const std::string& mode_name )
{
	if (!isRegisteredMode(mode_name))
	{
		mAvaiableModes.push_back(boost::to_upper_copy(mode_name));
	}
}

nsGameCore::EditMode::EditMode() :mModeName()
{

}

void nsGameCore::EditMode::addSubMode( const std::string& mode_name, const std::string& sub_mode_name )
{
	if (isRegisteredMode(mode_name))
	{
		//TODO
	}
}

bool nsGameCore::EditMode::hasSubMode( const std::string& mode_name, const std::string& sub_mode_name ) const
{
	//TODO
	return false;
}
