#include "CamResizeHandler.h"

renderer::CamResizeHandler::CamResizeHandler(osg::Camera* target_cam, const std::string& uniform_name)
    : mCamera(target_cam)
    , mUniformName(uniform_name)
{
}

bool renderer::CamResizeHandler::handle(const osgGA::GUIEventAdapter& gea, osgGA::GUIActionAdapter& gaa, osg::Object* obj, osg::NodeVisitor* nv)
{
    if (osgGA::GUIEventAdapter::EventType ev = gea.getEventType(); osgGA::GUIEventAdapter::RESIZE == ev)
    {
        auto width  = gea.getWindowWidth();
        auto height = gea.getWindowHeight();

        if (mCamera.valid())
        {
            mCamera->setViewport(0, 0, width, height);
            if (mCamera->getNumChildren())
            {
                //	Set projection
                mCamera->setProjectionMatrixAsOrtho2D(0, width, 0, height);
            }

            return true;
        }
    }
    return false;
}