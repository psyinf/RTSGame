#pragma once
#include "osg/Node"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "Properties.h"



namespace nsGameCore{
class GameModel;

class GameModelManager
{
	
	using ModelNodeMap = std::map<std::string, osg::ref_ptr<osg::Node>>;
	using GameModelMap = std::map<std::string, std::shared_ptr<GameModel>>;
	using ModelNames = std::set<std::string>;
public:
	GameModelManager(const std::string& path);

	~GameModelManager();

	void getRegisteredModelNames(std::vector<std::string>& model_names);

	bool loadModel(const std::string& model_name, const std::string& model_path);
	
	osg::ref_ptr<osg::Node> getModel(const std::string& model_name);

	std::shared_ptr<GameModel> createGameModelInstance(const std::string& model_name);
	//TODO: instances might be managed here
	//effectively each model should manage its properties
private:
	std::string		mModelPath;
	ModelNodeMap	mRegisteredModels;
	GameModelMap	mGameModelMap;
};

class GameModelFactory
{
public:

	static std::shared_ptr<GameModel> build(nsGameCore::GameModelManager& model_manager, const std::string& xml_description_file);
};


class GameModel
{
public:
	enum class Type 
	{
		MT_INVALID = 0,
		MT_BUILDING
	};

	virtual Type getModelType() const  = 0;

	GameModel(GameModelManager& game_model_manager, const std::string& model_type_name)
		:mrGameModelManager(game_model_manager)
		,mModelTypeName(model_type_name)
	{}
    virtual ~GameModel() = default;

	virtual osg::ref_ptr<osg::Node> getGraphicalModel() = 0;

	std::string getModelDescriptorPath() const;

	std::string getModelDisplayName() const;
	
	virtual void parse(const std::string& model_description_file);

	virtual std::shared_ptr<GameModel> clone() = 0;

	Properties& getProperties();

	osg::Matrix getPlacementMatrix() const;

	osg::Vec3 getModelScale() const;
protected:
	void loadGraphicalModel(const std::string& path);
	//XXX void addPropertyFromNode( nsXML::XMLNode &properties_node, std::string type );
protected:
	GameModelManager&		mrGameModelManager;	
	std::string				mModelTypeName;
	std::string				mModelDisplayName;
	osg::ref_ptr<osg::Node> mGraphicalModel;
	Properties				mProperties;
	osg::Matrix				mPlacementMatrix;
    osg::Vec3               mModelScale = {1,1,1};
};


class GameBuilding : public GameModel
{
	//TODO: construct from XML, containing at least the properties available for the Model type
public:
	GameBuilding(GameModelManager& game_model_manager, const std::string& model_type_name);

	~GameBuilding() override = default;

	void parse(const std::string& model_description_file) override;

	std::shared_ptr<GameModel> clone() override;

	Type getModelType() const override;

	osg::ref_ptr<osg::Node> getGraphicalModel() override;


};

}//namespace nsGameCore