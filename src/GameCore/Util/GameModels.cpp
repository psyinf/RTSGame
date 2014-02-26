#include "GameModels.h"
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>

#include <algorithm>
#include <iterator>
#include "Filesystem/Directory.h"
#include "FileFormats/XMLSerializer.h"

#include <osgDB/ReadFile>

#include <boost/algorithm/string.hpp>
#include <common/CustomShaderProgram.h>

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
	osg::ref_ptr<osg::Node> node = mrGameModelManager.getModel(mModelDisplayName);
	if (!node)
	{
		throw std::runtime_error("Model not registered " + mModelDisplayName);
	}
	osg::CopyOp::CopyFlags copy_flags = osg::CopyOp::SHALLOW_COPY;

	copy_flags = copy_flags | osg::CopyOp::DEEP_COPY_NODES;
	copy_flags = copy_flags | osg::CopyOp::DEEP_COPY_CALLBACKS;
	mGraphicalModel = dynamic_cast<osg::Node*>(node->clone(copy_flags));
	mGraphicalModel->getOrCreateStateSet()->getOrCreateUniform("ConstructionProgress", osg::Uniform::FLOAT)->set(0.0f);
	nsRenderer::CustomShaderProgram* program = nsRenderer::CustomShaderProgram::loadProgram("data/shaders/Building", nsRenderer::ShaderInfo());
	mGraphicalModel->getOrCreateStateSet()->setAttribute(program, osg::StateAttribute::ON);

	return mGraphicalModel;
}



void nsGameCore::GameModelManager::getRegisteredModelNames( std::vector<std::string>& model_names )
{
	std::transform(mGameModelMap.begin(), 
		mGameModelMap.end(), 
		std::back_inserter(model_names),
		boost::bind(&GameModelMap::value_type::first, _1)
	);
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
		boost::shared_ptr<nsGameCore::GameModel> model = GameModelFactory::build(*this, path + "/" + *iter);
		if (model)
		{
			mGameModelMap.insert(std::make_pair(model->getModelDisplayName(), model));
		}
		
	}

}

bool nsGameCore::GameModelManager::loadModel( const std::string& model_name, const std::string& model_path )
{
	osg::ref_ptr<osg::Node> ret_node = osgDB::readNodeFile(mModelPath + "/" + model_path);
	if (ret_node)
	{
		mRegisteredModels.insert(std::make_pair(model_name, ret_node));
		return true;
	}
	return false;
}

boost::shared_ptr<nsGameCore::GameModel> nsGameCore::GameModelManager::createGameModelInstance( const std::string& model_name )
{
	return mGameModelMap.at(model_name)->clone();
}

osg::ref_ptr<osg::Node> nsGameCore::GameModelManager::getModel( const std::string& model_name )
{
	auto iter = mRegisteredModels.find(model_name);
	if (iter !=  mRegisteredModels.end())
	{
		return (*iter).second;
	}
	return nullptr;
}

std::string nsGameCore::GameModel::getModelDescriptorPath() const
{
	return mModelTypeName;
}

std::string nsGameCore::GameModel::getModelDisplayName() const
{
	return mModelDisplayName;
}

nsGameCore::Properties& nsGameCore::GameModel::getProperties()
{
	return mProperties;
}

void nsGameCore::GameBuilding::parse( const std::string& model_description_file )
{

	boost::shared_ptr<nsXML::XMLData> xml_document(new nsXML::XMLData(model_description_file.c_str()));
	std::string model_path = xml_document->getNode().getValue<std::string>("GameBuilding/ModelName");
	std::string model_display_name = xml_document->getNode().getValue<std::string>("GameBuilding/DisplayName");
	mModelDisplayName = model_display_name;
	if (!mrGameModelManager.loadModel(model_display_name, model_path))
	{
		//TODO: report error
	}
	nsXML::XMLNode properties_node = xml_document->getNode().getNode("GameBuilding/Properties");
	{
		addPropertyFromNode(properties_node, "RunningCost");
		addPropertyFromNode(properties_node, "BuildingCost");
		addPropertyFromNode(properties_node, "Production");
		addPropertyFromNode(properties_node, "Status");
	}
}

void nsGameCore::GameBuilding::addPropertyFromNode( nsXML::XMLNode &properties_node, std::string type )
{
	std::vector<nsXML::XMLNode> build_cost_nodes = properties_node.getNodes("Properties/" + type );
	for (auto iter = build_cost_nodes.begin()
		;iter != build_cost_nodes.end()
		;++iter
		)
	{
		const nsXML::XMLNode& node = (*iter);
		std::string name = node.getAttrib<std::string>("name");
		std::string value = node.getAttrib<std::string>("value");
		getProperties().addProperty(type, name + ":" + value);
	}
}

boost::shared_ptr<nsGameCore::GameModel> 
nsGameCore::GameBuilding::clone()
{
	return GameModelFactory::build(mrGameModelManager, mModelTypeName);
}

nsGameCore::GameBuilding::GameBuilding( GameModelManager& game_model_manager, const std::string& model_type_name ) :GameModel(game_model_manager, model_type_name)
{
	parse( model_type_name);
}

boost::shared_ptr<nsGameCore::GameModel> 
nsGameCore::GameModelFactory::build( nsGameCore::GameModelManager& model_manager, const std::string& xml_description_file )
{
	try 
	{
		boost::scoped_ptr<nsXML::XMLData> xml_document (new nsXML::XMLData(xml_description_file));
		const std::string model_type_name = xml_document->getNode().name();
		if ("GameBuilding" == model_type_name)
		{
			boost::shared_ptr<GameModel> model(new GameBuilding(model_manager, xml_description_file));
			return model;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error loading xml data for: " << e.what() << std::endl;
	}
	return boost::shared_ptr<GameModel>();
	
}
