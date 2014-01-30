#pragma once
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>
#include <osgTerrain/Terrain>
#include <osgTerrain/GeometryTechnique>
#include <osgGA/GUIEventHandler>
#include "Core/Core.h"


#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

namespace osgTerrain {

	class ModifyingTerrainTechnique : public osgTerrain::GeometryTechnique
	{
	public:
		ModifyingTerrainTechnique(void);

		/** Copy constructor using CopyOp to manage deep vs shallow copy.*/
		ModifyingTerrainTechnique(const osgTerrain::GeometryTechnique& technique,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

		META_Object(osgTerrain, ModifyingTerrainTechnique);


		~ModifyingTerrainTechnique();

		virtual void init(int dirtyMask, bool assumeMultiThreaded);

		virtual void update( osgUtil::UpdateVisitor* nv );

		virtual void traverse( osg::NodeVisitor& nv );

		bool setHeight(double x, double y, double height);

		bool setHeight(double x, double y, double radius_ndc, double height);

		bool getHeight(double x, double y, double& height);

		bool getExtents(osg::Vec3d& top_left, osg::Vec3d& bottom_right);

	};

}

// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler {
public:

	PickHandler()
	{}

	  ~PickHandler() {}

	  bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	  virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	  

protected:
	
};

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	if (0 == (ea. getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT))
	{
		return false;
	}
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::PUSH):
		{
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view,ea);
			return false;
		}
	case(osgGA::GUIEventAdapter::DRAG):
		{
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view,ea);
			return true;
		}
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (ea.getKey()=='c')
			{
				osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
				osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
				event->setX((ea.getXmin()+ea.getXmax())*0.5);
				event->setY((ea.getYmin()+ea.getYmax())*0.5);
				if (view) pick(view,*event);
			}
			return false;
		}
	default:
		return false;
	}
}

void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
 	osgUtil::LineSegmentIntersector::Intersections intersections;

	std::string gdlist="";

	if (view->computeIntersections(ea,intersections))
	{
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
			hitr != intersections.end();
			++hitr)
		{

			for (osg::NodePath::const_iterator iter = (*hitr).nodePath.begin(); iter != (*hitr).nodePath.end(); ++iter)
			{
				osgTerrain::TerrainTile* terrain_tile = dynamic_cast<osgTerrain::TerrainTile*>((*iter));
				if (terrain_tile)
				{
					 osgTerrain::ModifyingTerrainTechnique* mod_technique = dynamic_cast<osgTerrain::ModifyingTerrainTechnique*>(terrain_tile->getTerrainTechnique());
					 if (mod_technique)
					 {
						 osg::Vec3d local_position;
						 osgTerrain::Locator* locator = terrain_tile->getElevationLayer()->getLocator();
						 if (locator)
						 {
							 locator->convertModelToLocal((*hitr).getWorldIntersectPoint(),local_position);
							 double height = 0.0;
							 mod_technique->getHeight(local_position[0], local_position[1], height);	 
							 mod_technique->setHeight(local_position[0], local_position[1], height + 0.25);	 
						 }
						 
					 }
				}
			}
		}
	}
	//setLabel(gdlist);
}


class FilterHandler : public osgGA::GUIEventHandler 
{
public: 

	FilterHandler(osgTerrain::GeometryTechnique* gt):
	  _gt(gt) {}

	  bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
	  {
		  if (!_gt) return false;

		  switch(ea.getEventType())
		  {
		  case(osgGA::GUIEventAdapter::KEYDOWN):
			  {
				  if (ea.getKey() == 'g')
				  {
					  osg::notify(osg::NOTICE)<<"Gaussian"<<std::endl;
					  _gt->setFilterMatrixAs(osgTerrain::GeometryTechnique::GAUSSIAN);
					  return true;
				  }
				  else if (ea.getKey() == 's')
				  {
					  osg::notify(osg::NOTICE)<<"Smooth"<<std::endl;
					  _gt->setFilterMatrixAs(osgTerrain::GeometryTechnique::SMOOTH);
					  return true;
				  }
				  else if (ea.getKey() == 'S')
				  {
					  osg::notify(osg::NOTICE)<<"Sharpen"<<std::endl;
					  _gt->setFilterMatrixAs(osgTerrain::GeometryTechnique::SHARPEN);
					  return true;
				  }
				  else if (ea.getKey() == '+')
				  {
					  _gt->setFilterWidth(_gt->getFilterWidth()*1.1);
					  osgTerrain::ModifyingTerrainTechnique* my_technique = dynamic_cast<osgTerrain::ModifyingTerrainTechnique*>(_gt.get());
					  if (my_technique)
					  {
						  double height;
						  my_technique->getHeight(0.5,0.5, height);
						  my_technique->setHeight(0.5,0.5,height + 0.5);
					  }
					  osg::notify(osg::NOTICE)<<"Filter width = "<<_gt->getFilterWidth()<<std::endl;
					  return true;
				  }
				  else if (ea.getKey() == '-')
				  {
					  _gt->setFilterWidth(_gt->getFilterWidth()/1.1);
					  osg::notify(osg::NOTICE)<<"Filter width = "<<_gt->getFilterWidth()<<std::endl;
					  osgTerrain::ModifyingTerrainTechnique* my_technique = dynamic_cast<osgTerrain::ModifyingTerrainTechnique*>(_gt.get());
					  if (my_technique)
					  {
						  double height;
						  my_technique->getHeight(0.5,0.5, height);
						  my_technique->setHeight(0.5,0.5,height - 0.5);
					  }
					  return true;
				  }
				  else if (ea.getKey() == '>')
				  {
					  _gt->setFilterBias(_gt->getFilterBias()+0.1);
					  osg::notify(osg::NOTICE)<<"Filter bias = "<<_gt->getFilterBias()<<std::endl;
					  return true;
				  }
				  else if (ea.getKey() == '<')
				  {
					  _gt->setFilterBias(_gt->getFilterBias()-0.1);
					  osg::notify(osg::NOTICE)<<"Filter bias = "<<_gt->getFilterBias()<<std::endl;
					  return true;
				  }
				  break;
			  }
		  default:
			  break;
		  }
		  return false;

	  }

protected:

	osg::observer_ptr<osgTerrain::GeometryTechnique> _gt;

};


//
namespace nsGameCore{ 




class Terrain
{
public:
	Terrain(nsRenderer::Core& ref_core);
	~Terrain();

	void load(const std::string& base_name);

	

	osg::ref_ptr<osg::Group> getTerrainNode();
protected:
	osg::ref_ptr<osgTerrain::Terrain> mTerrain;
	
	nsRenderer::Core&		 mrCore;



};
} //namespace