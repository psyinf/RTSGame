#pragma once
#include "osg/Node"
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


namespace boost {
	template<> 
	inline bool lexical_cast<bool, std::string>(const std::string& arg) 
	{
		std::istringstream ss(arg);
		bool b;
		ss >> std::boolalpha >> b;
		return b;
	}
	
/*
	template<>
	inline std::string lexical_cast<std::string, bool>(const bool& b) 
	{
		std::ostringstream ss;
		ss << std::boolalpha << b;
		return ss.str();
	}*/
}

namespace nsGameCore{
class GameModel;

class GameModelManager
{
	
	typedef std::map<std::string, osg::ref_ptr<osg::Node>> ModelNameMap;
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
	ModelNameMap	mRegisteredModels;
	ModelNames		mKnownModels;
	GameModelMap	mGameModelMap;
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

	std::string getModelTypeName() const;

	virtual void parse(const std::string& model_description_file) = 0;
protected:
	void loadGraphicalModel(const std::string& path);
protected:
	GameModelManager&		mrGameModelManager;	
	std::string				mModelTypeName;
	osg::ref_ptr<osg::Node> mGraphicalModel;
};


class GameBuilding : public GameModel
{
	//TODO: construct from XML, containing at least the properties available for the Model type
public:
	GameBuilding(GameModelManager& game_model_manager, const std::string& model_type_name)
		:GameModel(game_model_manager, model_type_name)
	{
		parse(model_type_name);
		if (boost::iequals("H3-Mine", model_type_name))
		{
			addProperty("Production", "H3:50");
			addProperty("BuildingCost", "CM:100,Energy:100");
			addProperty("RunningCost", "Energy:50");
			addProperty("IsProducing", "false");
		}
		else if (boost::iequals("Mine", model_type_name))
		{
			addProperty("Production", "Ores:10");
			addProperty("BuildingCost", "CM:100,Energy:100");
			addProperty("RunningCost", "Energy:100");
			addProperty("IsProducing", "false");
		}
		else if (boost::iequals("Oilwell", model_type_name))
		{
			addProperty("Production", "Energy:200");
			addProperty("BuildingCost", "CM:100,Energy:100");
			addProperty("RunningCost", "H3:30");
			addProperty("IsProducing", "false");
		}
	}

	virtual ~GameBuilding(){}

	virtual void parse(const std::string& model_description_file);

	virtual Type getModelType() const;

	virtual osg::ref_ptr<osg::Node> getGraphicalModel();

	std::string getProperty(const std::string& property_name) const;

	template <typename T>
	T getPropertyValue(const std::string& property_name)
	{
		return boost::lexical_cast<T>(getProperty(property_name));
	}

	void setProperty(const std::string& property_name, const std::string& property_value);

	std::vector<std::string> getSubProperties(const std::string& property_name) const;

	std::vector<std::string> getSubPropertyNames(const std::string& property_name) const;

	std::vector<std::pair<std::string, std::string>> getSubPropertyPairs(const std::string& property_name) const;

	template <typename T> 
	T getPropertySubValue(const std::string& property_name, const std::string& sub_name) const
	{
		std::vector<std::string> values = getSubProperties(property_name);

		for (auto iter = values.begin(); iter != values.end(); ++iter)
		{
			std::vector<std::string> value_split;
			boost::split(value_split, (*iter), boost::is_any_of(":"));
			if (value_split.size() == 2 && boost::iequals(value_split[0], sub_name ))
			{
				return boost::lexical_cast<T>(value_split[1]);
			}
		}
		throw std::runtime_error("Unknown property: ");
	}
protected:
	void addProperty(const std::string& name, const std::string& value_str)
	{
		mProperties[name]		= value_str;
	}
protected:
	std::map<std::string, std::string> mProperties;


};

}//namespace nsGameCore