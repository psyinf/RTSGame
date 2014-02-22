#pragma once
#include "Util/Properties.h"
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
#include <map>

#include <osg/Camera>
#include <osgText/Text>

namespace nsRenderer
{
	class Core;
}
namespace nsGameCore{

class Terrain;
class GameArea;
class GameModelManager;
class GameModel;
class HUDManager;
struct CellData;
struct CellAdress;
class GameLogic;

typedef boost::shared_ptr<CellData> CellDataPtr;



struct NamedValue
{
	enum ValueType 
	{
		VT_INTEGRAL,
		VT_FLOAT,
		VT_STRING,
		VT_BOOL,
	};


	NamedValue()
		:name("")
		,type(VT_STRING)
	{

	}
	NamedValue(const std::string& name, const ValueType type)
		:name(name)
	{
		switch (type)
		{
		case VT_INTEGRAL:
			value = boost::lexical_cast<std::string>(0);
			break;
		case VT_FLOAT:
			value = boost::lexical_cast<std::string>(0);
			break;
		case VT_BOOL:
			value = boost::lexical_cast<std::string>(false);
			break;
		default: //fall through
		case VT_STRING:
			value = "";
			break;
		}
	}
	template <class T>  
	T getValue() const
	{
		return boost::lexical_cast<T>(value);
	}


	template <class T>  
	void setValue(T v)
	{
		value = boost::lexical_cast<std::string>(v);
	}

	std::string getValueString() const
	{
		return value;
	}

protected:
	std::string name;
	std::string value;
	ValueType type;
};

template <class T>
class ScopedNamedValue
{
public:
	ScopedNamedValue(NamedValue& named_value)
		:mrNamedValue(named_value)
		,mValue(named_value.getValue<T>())
		,mPotentiallyChanged(false)
	{

	}

	~ScopedNamedValue()
	{
		if (mPotentiallyChanged)
		{
			mrNamedValue.setValue<T>(mValue);
		}
	}

	T getValue()
	{
		return mValue;
	}

	T& getValueRef()
	{
		mPotentiallyChanged = true;
		return mValue;
	}
protected:
	bool mPotentiallyChanged;
	T mValue;
	NamedValue& mrNamedValue;
};

struct EditMode
{
	EditMode();
	void addRegisteredMode(const std::string& mode_name);
	
	bool isRegisteredMode(const std::string& mode_name) const;

	std::string nextMode();

	bool setMode(const std::string& new_mode_name);

	std::string getCurrentModeName() const;

	void addSubMode(const std::string& mode_name, const std::string& sub_mode_name);

	bool hasSubMode(const std::string& mode_name, const std::string& sub_mode_name) const;

	std::string getSubMode() const;

	void setSubMode(const std::string& sub_mode_name);
	
	std::string nextSubMode();


protected:
	std::string mModeName;
	std::string mSubModeName;
	std::map<std::string,std::vector<std::string>>	mSubModes;
	std::vector<std::string>						mAvailableModes;

};

class GameCore
{
public:
	GameCore(nsRenderer::Core& render_core);
	~GameCore();

	void setup(const std::string& configuration);

	void frame();

	boost::shared_ptr<GameModel> placeModel(osg::Vec3d& position, osg::Quat& orientation, osg::Vec3 scale, const std::string& model_type);

	CellDataPtr getCellData(const CellAdress& address);

	void setCellData(const CellAdress& address, CellDataPtr cell_data_ptr);
	


	nsRenderer::Core& getRenderCore();

	nsGameCore::GameModelManager& getModelManager();
	
	EditMode& getCurrentEditMode();

	const EditMode& getCurrentEditMode() const;

	void setCurrentEditMode(const EditMode& edit_mode);

	void setModeAndEditMode(const std::string& mode, const std::string& sub_mode );
	
	osg::Camera* getHUDCamera();

	osgText::Text* getNamedTextObject(const std::string& );

	void createNamedTextObject(const std::string&, osg::ref_ptr<osgText::Text> text_node );

	CellAdress calculateCellAdress(float x, float y, unsigned int level = 0);
	
	NamedValue& getNamedValue(const std::string& name);

	nsGameCore::NamedValue& addNamedValue(const std::string& name, const NamedValue::ValueType& value_type );
	
	boost::shared_ptr<nsGameCore::HUDManager> getHUDManager();

	boost::shared_ptr<nsGameCore::GameArea>	getGameArea();
protected:
	EditMode											mCurrentEditMode;
	nsRenderer::Core&									mrCore;
	boost::shared_ptr<nsGameCore::Terrain>				mTerrain;
	boost::shared_ptr<nsGameCore::GameArea>				mGameArea;
	boost::shared_ptr<nsGameCore::GameModelManager>		mModelManager;
	boost::shared_ptr<nsGameCore::GameLogic>			mGameLogic;


	//those HUD things should be refactored to a separate module
	osg::ref_ptr<osg::Camera>							mHUDCamera;
	std::map<std::string, osg::ref_ptr<osgText::Text>> mHUDTextElements;
	osg::ref_ptr<osg::Geode> mTextNode;
	boost::shared_ptr<nsGameCore::HUDManager>			mHUDManager;

	std::map<std::string, NamedValue>				mNamedValues;
	Properties										mProperties;

	
};


} //nsGameCore