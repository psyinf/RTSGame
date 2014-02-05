#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

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
class HUDManager;
struct CellData;
struct CellAdress;

typedef boost::shared_ptr<CellData> CellDataPtr;


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

	void setModel(CellDataPtr& cell_data, const std::string& model_type  );
	
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

	void placeModel(osg::Vec3d& position, osg::Quat& orientation, osg::Vec3 scale, const std::string& model_type);

	CellDataPtr getCellData(const CellAdress& address);

	void setCellData(const CellAdress& address, CellDataPtr cell_data_ptr);
	


	nsRenderer::Core& getRenderCore();

	nsGameCore::GameModelManager& getModelManager();
	
	EditMode& getCurrentEditMode();

	const EditMode& getCurrentEditMode() const;

	void setCurrentEditMode(const EditMode& edit_mode);

	
	
	osg::Camera* getHUDCamera();

	osgText::Text* getNamedTextObject(const std::string& );

	void createNamedTextObject(const std::string&, osg::ref_ptr<osgText::Text> text_node );

	CellAdress calculateCellAdress(float x, float y, unsigned int level = 0);

protected:
	EditMode											mCurrentEditMode;
	nsRenderer::Core&									mrCore;
	boost::shared_ptr<nsGameCore::Terrain>				mTerrain;
	boost::shared_ptr<nsGameCore::GameArea>				mGameArea;
	boost::shared_ptr<nsGameCore::GameModelManager>		mModelManager;


	//those HUD things should be refactored to a separate module
	osg::ref_ptr<osg::Camera>							mHUDCamera;
	std::map<std::string, osg::ref_ptr<osgText::Text>> mHUDTextElements;
	osg::ref_ptr<osg::Geode> mTextNode;
	boost::shared_ptr<nsGameCore::HUDManager>			mHUDManager;

	
};


} //nsGameCore