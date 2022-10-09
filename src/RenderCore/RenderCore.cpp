#include "RenderCore.h"
#include <osgDB/Registry>

using namespace renderer;
RenderCore::RenderCore()
{
    createSubRoot("MAIN_ROOT", true);
    createSubRoot("MODEL_ROOT", true);
    createSubRoot("TERRAIN_ROOT", true);


    viewer->setSceneData(getMainRoot());
    viewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    viewer->setUpViewInWindow(100, 100, 1024, 1024);
   
}

osg::Group* RenderCore::getSubRoot(const std::string& name)
{
    return namedRoots.at(name);
}

osg::Group* RenderCore::getMainRoot()
{
	return mainRoot;
}

void RenderCore::createSubRoot(const std::string& name, bool addToMain)
{
    if (namedRoots.contains(name))
    {
        //TODO: log
        return;
    }
    osg::ref_ptr<osg::Group> group = new osg::Group;
    group->setName(name);
    namedRoots.try_emplace(name, group);

    if (addToMain)
    {
        getMainRoot()->addChild(group);
    }
}



osgViewer::Viewer* RenderCore::getViewer()
{
    return viewer;
}

void RenderCore::setup()
{

    viewer->realize();
   
}

void RenderCore::frame()
{
    viewer->advance(); // ref time increase
  //  double time_elapsed_rt = mMainViewer->getFrameStamp()->getReferenceTime() - last_stamp_rt;
  //  viewer->getFrameStamp()->setSimulationTime(last_stamp_st + (time_elapsed_rt * mSpeedFactor));
    viewer->eventTraversal();
    viewer->updateTraversal();
   

    viewer->renderingTraversals();
}
USE_OSGPLUGIN(gdal)