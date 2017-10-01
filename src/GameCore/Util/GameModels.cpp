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
#include <common/CustomShader/CustomShaderProgram.h>


void nsGameCore::GameModel::addPropertyFromNode( nsXML::XMLNode &properties_node, std::string type )
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
		std::cerr << "Error: Graphical Model not registered: " << mModelDisplayName << std::endl;
		mGraphicalModel = osg::ref_ptr<osg::Node>(new osg::Node());
		return mGraphicalModel;
		//throw std::runtime_error("Model not registered " + mModelDisplayName);
	}
	osg::CopyOp::CopyFlags copy_flags = osg::CopyOp::SHALLOW_COPY;

	copy_flags = copy_flags | osg::CopyOp::DEEP_COPY_NODES;
	copy_flags = copy_flags | osg::CopyOp::DEEP_COPY_CALLBACKS;
	mGraphicalModel = dynamic_cast<osg::Node*>(node->clone(copy_flags));
	//TODO: move this to a separate function to make it configurable
	mGraphicalModel->getOrCreateStateSet()->getOrCreateUniform("ConstructionProgress", osg::Uniform::FLOAT)->set(0.0f);
	nsRenderer::ShaderProgram* program = nsRenderer::ShaderProgramHelpers::loadProgram("data/shaders/Building", nsRenderer::ShaderInfo());
	//mGraphicalModel->getOrCreateStateSet()->setAttribute(program, osg::StateAttribute::ON);

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

void nsGameCore::GameModel::parse( const std::string& model_description_file )
{
	boost::shared_ptr<nsXML::XMLData> xml_document(new nsXML::XMLData(model_description_file.c_str()));
	std::string model_path = xml_document->getNode().getValue<std::string>("GameModel/ModelName");
	std::string model_display_name = xml_document->getNode().getValue<std::string>("GameModel/DisplayName");
	mModelDisplayName = model_display_name;
	if (!mrGameModelManager.loadModel(model_display_name, model_path))
	{
		//TODO: report error
	}
	nsXML::XMLNode properties_node = xml_document->getNode().getNode("GameModel/Properties");
	{
		addPropertyFromNode(properties_node, "RunningCost");
		addPropertyFromNode(properties_node, "BuildingCost");
		addPropertyFromNode(properties_node, "Production");
		addPropertyFromNode(properties_node, "Status");
	}
	try 
	{
		nsXML::XMLNode offset_node = xml_document->getNode().getNode("GameModel/Offset");
		if (!offset_node.empty())
		{
			std::string offset_string = offset_node.getValue<std::string>("Offset");
			std::vector<std::string> values;
			osg::Vec3 offset_vec;
			boost::split(values, offset_string, boost::is_any_of(", "), boost::algorithm::token_compress_on);
			if (3 == values.size())
			{
				offset_vec[0] = boost::lexical_cast<float>(values[0]);
				offset_vec[1] = boost::lexical_cast<float>(values[1]);
				offset_vec[2] = boost::lexical_cast<float>(values[2]);
			}

			mPlacementMatrix.setTrans(offset_vec);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error getting Offset " << e.what();
	}


	try 
	{
		nsXML::XMLNode rot_node = xml_document->getNode().getNode("GameModel/Rotate");
		if (!rot_node.empty())
		{
			std::string offset_string = rot_node.getValue<std::string>("Rotate");
			std::vector<std::string> values;
			osg::Vec3 hpr_vec;
			boost::split(values, offset_string, boost::is_any_of(", "), boost::algorithm::token_compress_on);
			if (3 == values.size())
			{
				hpr_vec[0] = osg::DegreesToRadians(boost::lexical_cast<float>(values[0]));
				hpr_vec[1] = osg::DegreesToRadians(boost::lexical_cast<float>(values[1]));
				hpr_vec[2] = osg::DegreesToRadians(boost::lexical_cast<float>(values[2]));
			}
			osg::Quat rot_quat(hpr_vec[0], osg::Vec3(0,0,1), hpr_vec[1], osg::Vec3(1,0,0),hpr_vec[2], osg::Vec3(0,1,0));

			mPlacementMatrix.setRotate(rot_quat);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error getting Rotation " << e.what();
	}


	try 
	{
		nsXML::XMLNode rot_node = xml_document->getNode().getNode("GameModel/Scale");
		if (!rot_node.empty())
		{
			std::string scale_string = rot_node.getValue<std::string>("Scale");
			std::vector<std::string> values;
			osg::Vec3 scale_vec(1,1,1);
			boost::split(values, scale_string, boost::is_any_of(", "), boost::algorithm::token_compress_on);
			if (3 == values.size())
			{
				scale_vec[0] = (boost::lexical_cast<float>(values[0]));
				scale_vec[1] = (boost::lexical_cast<float>(values[1]));
				scale_vec[2] = (boost::lexical_cast<float>(values[2]));
			}
			mModelScale = scale_vec;
			
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error getting Scale " << e.what();
	}
	
	
}

osg::Matrix nsGameCore::GameModel::getPlacementMatrix() const
{
	return mPlacementMatrix;
}

osg::Vec3 nsGameCore::GameModel::getModelScale() const
{
	return mModelScale;
}

void nsGameCore::GameBuilding::parse( const std::string& model_description_file )
{
	GameModel::parse(model_description_file);
	
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
		if ("GameModel" == model_type_name)
		{
			boost::shared_ptr<GameModel> model(new GameBuilding(model_manager, xml_description_file));
			return model;
		}
		else
		{
			throw std::runtime_error("Unsupported Model type" + model_type_name);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error loading xml data for: " << e.what() << std::endl;
	}
	return boost::shared_ptr<GameModel>();
	
}
