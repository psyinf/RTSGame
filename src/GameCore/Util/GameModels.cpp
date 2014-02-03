#include "GameModels.h"
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iterator>
#include "Filesystem/Directory.h"

#include <osgDB/ReadFile>

#include <boost/algorithm/string.hpp>


nsGameCore::GameModel::Type 
nsGameCore::GameBuilding::getModelType() const
{
	return GameModel::MT_BUILDING;
}

osg::Node* 
nsGameCore::GameBuilding::getGraphicalModel()
{
	//TODO
	return nullptr;
}

void nsGameCore::GameModelManager::getRegisteredModelNames( std::vector<std::string>& model_names )
{
	std::transform(mRegisteredModels.begin(), 
		mRegisteredModels.end(), 
		std::back_inserter(model_names),
		boost::bind(&ModelNameMap::value_type::first, _1)
		);
}

nsGameCore::GameModelManager::~GameModelManager()
{

}

nsGameCore::GameModelManager::GameModelManager( const std::string& path )
{
	std::vector<std::string> ret_files;
	Directory::getFilesInDir(path, ret_files, "*.obj" );
	for (auto iter = ret_files.begin(); iter != ret_files.end(); ++iter)
	{
		osg::ref_ptr<osg::Node> ret_node = osgDB::readNodeFile(path + "/" + (*iter));
		if (ret_node)
		{
			mRegisteredModels.insert(std::make_pair(boost::to_upper_copy(Directory::getFileName((*iter), false)), ret_node));
		}
	}

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
