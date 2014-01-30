#pragma once
#include <osgTerrain/GeometryTechnique>


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