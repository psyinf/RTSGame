#pragma once
#include "osg/Node"

namespace nsGameCore{

class GameModelManager
{
	
	typedef std::map<std::string, osg::ref_ptr<osg::Node>> ModelNameMap;
public:
	GameModelManager(const std::string& path);

	~GameModelManager();

	void getRegisteredModelNames(std::vector<std::string>& model_names);

	osg::ref_ptr<osg::Node> getModel(const std::string& model_name);

	//TODO: instances might be managed here
	//effectively each model should manage its properties
protected:
	ModelNameMap mRegisteredModels;
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

	GameModel(GameModelManager& game_model_manager)
		:mrGameModelManager(game_model_manager)
	{}
	virtual ~GameModel() {}

	virtual osg::Node* getGraphicalModel() = 0;
protected:
	GameModelManager& mrGameModelManager;	

};


class GameBuilding : public GameModel
{
public:
	GameBuilding(GameModelManager& game_model_manager)
		:GameModel(game_model_manager)
	{}

	virtual ~GameBuilding(){}

	virtual Type getModelType() const;

	virtual osg::Node* getGraphicalModel();


};

}//namespace nsGameCore