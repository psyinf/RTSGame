#include "GameModels.h"
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iterator>
#include "Filesystem/Directory.h"
#include "FileFormats/XMLSerializer.h"

#include <osgDB/ReadFile>

#include <boost/algorithm/string.hpp>


nsGameCore::GameModel::Type 
nsGameCore::GameBuilding::getModelType() const
{
	return GameModel::MT_BUILDING;
}

osg::ref_ptr<osg::Node>
nsGameCore::GameBuilding::getGraphicalModel()
{
	if (mGraphicalModel)
	{
		return mGraphicalModel;
	}
	osg::ref_ptr<osg::Node> node = mrGameModelManager.getModel(mModelTypeName);
	if (!node)
	{
		throw std::runtime_error("Model not registered " + mModelTypeName);
	}
	osg::CopyOp::CopyFlags copy_flags = osg::CopyOp::SHALLOW_COPY;

	copy_flags = copy_flags | osg::CopyOp::DEEP_COPY_NODES;
	copy_flags = copy_flags | osg::CopyOp::DEEP_COPY_CALLBACKS;
	mGraphicalModel = dynamic_cast<osg::Node*>(node->clone(copy_flags));

	return mGraphicalModel;
}

std::string nsGameCore::GameBuilding::getProperty( const std::string& property_name ) const
{
	const std::string canonical_name = property_name;
	auto iter = mProperties.find(canonical_name);
	if (iter != mProperties.end())
	{
		return (*iter).second;
	}
	return "";
	//throw std::runtime_error("Unknown Property: " + canonical_name);
}

void nsGameCore::GameBuilding::setProperty( const std::string& property_name, const std::string& property_value )
{
	const std::string canonical_name = property_name;
	if (mProperties.count(canonical_name))
	{
		mProperties[canonical_name] = property_value;
	}
	else
	{
		throw std::runtime_error("Unknown Property: " + canonical_name);
	}
}

std::vector<std::string> nsGameCore::GameBuilding::getSubProperties( const std::string& property_name ) const
{
	std::string property_value_str = getProperty(property_name);
	std::vector<std::string> values;
	boost::split(values, property_value_str, boost::is_any_of(","));
	return values;
}

std::vector<std::pair<std::string, std::string>> nsGameCore::GameBuilding::getSubPropertyPairs( const std::string& property_name ) const
{
	std::vector<std::pair<std::string,std::string>> result_pairs;
	std::vector<std::string> sub_props = getSubProperties(property_name);
	for (auto iter = sub_props.begin(); iter != sub_props.end(); ++iter)
	{
		std::vector<std::string> split_result;
		boost::split(split_result, *iter, boost::is_any_of(":"));
		if (split_result.size() >= 2)
		{
			result_pairs.push_back(std::make_pair(split_result[0], split_result[1]));
		}
	}
	return result_pairs;
}

std::vector<std::string> nsGameCore::GameBuilding::getSubPropertyNames( const std::string& property_name ) const
{
	std::vector<std::string> result_names;
	std::vector<std::string> sub_props = getSubProperties(property_name);
	for (auto iter = sub_props.begin(); iter != sub_props.end(); ++iter)
	{
		std::vector<std::string> split_result;
		boost::split(split_result, *iter, boost::is_any_of(":"));
		if (split_result.size() >= 1 && !split_result[0].empty())
		{
			result_names.push_back(split_result[0]);
		}
	}
	return result_names;
}

void nsGameCore::GameModelManager::getRegisteredModelNames( std::vector<std::string>& model_names )
{
	/*std::for_each(mRegisteredModels.begin(), 
		mRegisteredModels.end(), 
		std::back_inserter(model_names),
		boost::bind(&ModelNames::value_type, _1)
		);*/
	model_names.assign(mKnownModels.begin(), mKnownModels.end());
}

nsGameCore::GameModelManager::~GameModelManager()
{

}

nsGameCore::GameModelManager::GameModelManager( const std::string& path )
	:mModelPath(path)
{
	
	std::vector<std::string> ret_files;
	Directory::getFilesInDir(path, ret_files, "*.xml" );
	for (auto iter = ret_files.begin(); iter != ret_files.end(); ++iter)
	{
		mKnownModels.insert((*iter));
	}

}

bool nsGameCore::GameModelManager::loadModel( const std::string& model_name, const std::string& model_path )
{
	osg::ref_ptr<osg::Node> ret_node = osgDB::readNodeFile(model_path);
	if (ret_node)
	{
		mRegisteredModels.insert(std::make_pair(model_name, ret_node));
		return true;
	}
	return false;
}

boost::shared_ptr<nsGameCore::GameModel> nsGameCore::GameModelManager::createGameModelInstance( const std::string& model_name )
{
	//TODO: use a factory
	boost::shared_ptr<GameModel> game_model;
	game_model = boost::shared_ptr<GameBuilding>(new GameBuilding(*this, mModelPath + "/" +model_name));
	return game_model;
}

osg::ref_ptr<osg::Node> nsGameCore::GameModelManager::getModel( const std::string& model_name )
{
	auto iter = mRegisteredModels.find(boost::to_upper_copy(model_name));
	if (iter !=  mRegisteredModels.end())
	{
		return (*iter).second;
	}
	return nullptr;
}

std::string nsGameCore::GameModel::getModelTypeName() const
{
	return mModelTypeName;
}

void nsGameCore::GameBuilding::parse( const std::string& model_description_file )
{
	tinyxml2::XMLDocument document;
	if (0 != document.LoadFile(model_description_file.c_str()))
	{
		throw std::runtime_error("Error in model xml:");
	}
	std::string model_path;
	
	nsXML::getValue(model_path, "GameBuilding/ModelName", document);
	if (!mrGameModelManager.loadModel(mModelTypeName, model_path))
	{
		//TODO: report error
	}


}
