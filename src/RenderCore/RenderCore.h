#pragma once
#include <osg/Group>
#include <osgViewer/Viewer>
#include <string_view>

namespace renderer{
	
class RenderCore
{
public:

    RenderCore();
    osg::Group*               getMainRoot();
    osg::Group*              getSubRoot(const std::string& name);
    osgViewer::Viewer*        getViewer();
    void                     createSubRoot(const std::string& name, bool addToMainRoot);
    void                      frame();

	void setup();



private:


    std::map<std::string, osg::ref_ptr<osg::Group>> namedRoots;
    
   
    osg::ref_ptr<osg::Group> mainRoot = new osg::Group;
   
    osg::ref_ptr<osgViewer::Viewer> viewer =  new osgViewer::Viewer;


};	

} //namespace renderer