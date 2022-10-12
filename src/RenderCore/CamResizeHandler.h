#pragma once
#include <osgGA/GUIEventHandler>

namespace renderer
{

class CamResizeHandler : public osgGA::GUIEventHandler
{
public:
    CamResizeHandler(osg::Camera* target_cam, const std::string& uniform_name = "ViewPortSize");

    virtual bool handle(
        const osgGA::GUIEventAdapter&,
        osgGA::GUIActionAdapter&,
        osg::Object*,
        osg::NodeVisitor*) override;

protected:
    osg::observer_ptr<osg::Camera> mCamera;
    std::string                    mUniformName;
};

} // namespace renderer 