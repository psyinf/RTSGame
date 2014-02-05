#pragma once
#include "osg/Node"
#include <boost/shared_ptr.hpp>

namespace nsGameCore{
class GameModel;

class GameModelManager
{
	
	typedef std::map<std::string, osg::ref_ptr<osg::Node>> ModelNameMap;
	typedef std::map<std::string, boost::shared_ptr<GameModel>> GameModelMap;
public:
	GameModelManager(const std::string& path);

	~GameModelManager();

	void getRegisteredModelNames(std::vector<std::string>& model_names);

	osg::ref_ptr<osg::Node> getModel(const std::string& model_name);

	boost::shared_ptr<GameModel> createGameModelInstance(const std::string& model_name);
	//TODO: instances might be managed here
	//effectively each model should manage its properties
protected:
	ModelNameMap mRegisteredModels;
	GameModelMap mGameModelMap;
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
protected:
	GameModelManager&		mrGameModelManager;	
	std::string				mModelTypeName;
	osg::ref_ptr<osg::Node> mGraphicalModel;
};


class GameBuilding : public GameModel
{
public:
	GameBuilding(GameModelManager& game_model_manager, const std::string& model_type_name)
		:GameModel(game_model_manager, model_type_name)
	{}

	virtual ~GameBuilding(){}

	virtual Type getModelType() const;

	virtual osg::ref_ptr<osg::Node> getGraphicalModel();


};

}//namespace nsGameCore