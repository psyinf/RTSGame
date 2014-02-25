#pragma once
#include "osg/Node"
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "Properties.h"

namespace nsXML
{
	class XMLNode;
};
/*
namespace boost {
	template<> 
	inline bool lexical_cast<bool, std::string>(const std::string& arg) 
	{
		std::istringstream ss(arg);
		bool b;
		ss >> std::boolalpha >> b;
		return b;
	}

	template<>
	std::string lexical_cast<std::string, bool>(const bool& b) 
	{
		std::ostringstream ss;
		ss << std::boolalpha << b;
		return ss.str();
	}

}
*/
namespace nsGameCore{
class GameModel;

class GameModelManager
{
	
	typedef std::map<std::string, osg::ref_ptr<osg::Node>> ModelNodeMap;
	typedef std::map<std::string, boost::shared_ptr<GameModel>> GameModelMap;
	typedef std::set<std::string> ModelNames;
public:
	GameModelManager(const std::string& path);

	~GameModelManager();

	void getRegisteredModelNames(std::vector<std::string>& model_names);

	bool loadModel(const std::string& model_name, const std::string& model_path);
	
	osg::ref_ptr<osg::Node> getModel(const std::string& model_name);

	boost::shared_ptr<GameModel> createGameModelInstance(const std::string& model_name);
	//TODO: instances might be managed here
	//effectively each model should manage its properties
protected:
	std::string		mModelPath;
	ModelNodeMap	mRegisteredModels;
	GameModelMap	mGameModelMap;
};

class GameModelFactory
{
public:

	static boost::shared_ptr<GameModel> build(nsGameCore::GameModelManager& model_manager, const std::string& xml_description_file);
};


class GameModel
{
public:
	enum Type 
	{
		MT_INVALID = 0,
		MT_BUILDING
	};

	virtual Type getModelType() const  = 0;

	GameModel(GameModelManager& game_model_manager, const std::string& model_type_name)
		:mrGameModelManager(game_model_manager)
		,mModelTypeName(model_type_name)
	{}
	virtual ~GameModel() {}

	virtual osg::ref_ptr<osg::Node> getGraphicalModel() = 0;

	std::string getModelDescriptorPath() const;

	std::string getModelDisplayName() const;

	virtual void parse(const std::string& model_description_file) = 0;

	virtual boost::shared_ptr<GameModel> clone() = 0;

	Properties& getProperties();

protected:
	void loadGraphicalModel(const std::string& path);
protected:
	GameModelManager&		mrGameModelManager;	
	std::string				mModelTypeName;
	std::string				mModelDisplayName;
	osg::ref_ptr<osg::Node> mGraphicalModel;
	Properties				mProperties;
};


class GameBuilding : public GameModel
{
	//TODO: construct from XML, containing at least the properties available for the Model type
public:
	GameBuilding(GameModelManager& game_model_manager, const std::string& model_type_name);

	virtual ~GameBuilding(){}

	virtual void parse(const std::string& model_description_file);

	virtual boost::shared_ptr<GameModel> clone();

	virtual Type getModelType() const;

	virtual osg::ref_ptr<osg::Node> getGraphicalModel();

	
protected:
	void addPropertyFromNode( nsXML::XMLNode &properties_node, std::string type );

	
protected:



};

}//namespace nsGameCore