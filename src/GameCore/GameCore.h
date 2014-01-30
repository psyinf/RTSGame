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

struct EditMode
{
	EditMode();
	void addRegisteredMode(const std::string& mode_name);
	
	bool isRegisteredMode(const std::string& mode_name);

	std::string nextMode();

	bool setMode(const std::string& new_mode_name);

	std::string getCurrentModeName() const;

protected:
	std::string mModeName;
	

	std::vector<std::string> mAvaiableModes;
};

class GameCore
{
public:
	GameCore(nsRenderer::Core& render_core);
	~GameCore();

	void setup(const std::string& configuration);

	void frame();

	void placeModel(osg::Vec3d& position, osg::Quat& orientation, const std::string& model_type);

	nsRenderer::Core& getRenderCore();
	
	EditMode& getCurrentEditMode();

	const EditMode& getCurrentEditMode() const;

	void setCurrentEditMode(const EditMode& edit_mode);

	osg::Camera* getHUDCamera();

	osgText::Text* getNamedTextObject(const std::string& );
	void createNamedTextObject(const std::string&, osg::ref_ptr<osgText::Text> text_node );

protected:
	EditMode mCurrentEditMode;
	nsRenderer::Core& mrCore;
	osg::ref_ptr<osg::Camera> mHUDCamera;
	boost::shared_ptr<nsGameCore::Terrain> mTerrain;
	std::map<std::string, osg::ref_ptr<osgText::Text>> mHUDTextElements;
	osg::ref_ptr<osg::Geode> mTextNode;

	
};


} //nsGameCore