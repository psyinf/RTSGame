/* OpenSceneGraph example, osgterrain.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include <osg/ArgumentParser>
#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <osgTerrain/Terrain>
#include <osgTerrain/TerrainTile>
#include <osgTerrain/GeometryTechnique>
#include <osgTerrain/DisplacementMappingTechnique>
#include <osgTerrain/Layer>
#include <osgDB/FileNameUtils>

#include <osgFX/MultiTextureControl>


#include <iostream>

template<class T>
class FindTopMostNodeOfTypeVisitor : public osg::NodeVisitor
{
public:
    FindTopMostNodeOfTypeVisitor():
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
        _foundNode(0)
    {}

    void apply(osg::Node& node)
    {
        T* result = dynamic_cast<T*>(&node);
        if (result)
        {
            _foundNode = result;
        }
        else
        {
            traverse(node);
        }
    }

    T* _foundNode;
};

template<class T>
T* findTopMostNodeOfType(osg::Node* node)
{
    if (!node) return 0;

    FindTopMostNodeOfTypeVisitor<T> fnotv;
    node->accept(fnotv);

    return fnotv._foundNode;
}

// class to handle events with a pick
class TerrainHandler : public osgGA::GUIEventHandler {
public:

    TerrainHandler(osgTerrain::Terrain* terrain, osgFX::MultiTextureControl* mtc):
        _terrain(terrain),
        _mtc(mtc) {}

    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& /*aa*/)
    {
        switch(ea.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                if (ea.getKey()=='r')
                {
                    _terrain->setSampleRatio(_terrain->getSampleRatio()*0.5);
                    osg::notify(osg::NOTICE)<<"Sample ratio "<<_terrain->getSampleRatio()<<std::endl;
                    return true;
                }
                else if (ea.getKey()=='R')
                {
                    _terrain->setSampleRatio(_terrain->getSampleRatio()/0.5);
                    osg::notify(osg::NOTICE)<<"Sample ratio "<<_terrain->getSampleRatio()<<std::endl;
                    return true;
                }
                else if (ea.getKey()=='v')
                {
                    _terrain->setVerticalScale(_terrain->getVerticalScale()*1.25);
                    osg::notify(osg::NOTICE)<<"Vertical scale "<<_terrain->getVerticalScale()<<std::endl;
                    return true;
                }
                else if (ea.getKey()=='V')
                {
                    _terrain->setVerticalScale(_terrain->getVerticalScale()/1.25);
                    osg::notify(osg::NOTICE)<<"Vertical scale "<<_terrain->getVerticalScale()<<std::endl;
                    return true;
                }
                else if (ea.getKey()=='!') // shift 1
                {
                    assignTextureWeightToSingleTextureUnit(1);
                    return true;
                }
                else if (ea.getKey()=='"') // shift 1
                {
                    assignTextureWeightToSingleTextureUnit(2);
                    return true;
                }
                else if (ea.getKey()==')') // shift 1
                {
                    assignTextureWeightToSingleTextureUnit(0);
                    return true;
                }
                else if (ea.getKey()=='A')
                {
                    assignedToAll();
                    return true;
                }
                else if (ea.getKey()=='l')
                {
                    toggleDefine("LIGHTING");
                    return true;
                }
                else if (ea.getKey()=='h')
                {
                    toggleDefine("HEIGHTFIELD_LAYER");
                    return true;
                }
                else if (ea.getKey()=='t')
                {
                    toggleDefine("TEXTURE_2D");
                    return true;
                }
                else if (ea.getKey()=='y')
                {
                    toggleDefine("COLOR_LAYER0");
                    return true;
                }
                else if (ea.getKey()=='u')
                {
                    toggleDefine("COLOR_LAYER1");
                    return true;
                }
                else if (ea.getKey()=='i')
                {
                    toggleDefine("COLOR_LAYER2");
                    return true;
                }
                else if (ea.getKey()=='d')
                {
                    toggleDefine("COMPUTE_DIAGONALS", osg::StateAttribute::OFF);
                    return true;
                }

                return false;
            }
            default:
                return false;
        }
    }

    void toggleDefine(const std::string& defineName, int expectedDefault=osg::StateAttribute::ON)
    {
        osg::StateSet::DefineList& defineList = _terrain->getOrCreateStateSet()->getDefineList();
        osg::StateSet::DefineList::iterator itr = defineList.find(defineName);
        if (itr==defineList.end())
        {
            defineList[defineName].second = (expectedDefault | osg::StateAttribute::OVERRIDE); // assume the defines start off.
            itr = defineList.find(defineName);
        }

        osg::StateSet::DefinePair& dp = itr->second;
        if ( (dp.second & osg::StateAttribute::ON)==0) dp.second = (osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        else dp.second = (osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);


    }

protected:

    ~TerrainHandler() {}

    void assignTextureWeightToSingleTextureUnit(unsigned int unit)
    {
        if (!_mtc) return;
        for(unsigned int i=0; i<_mtc->getNumTextureWeights(); ++i)
        {
            _mtc->setTextureWeight(i, (i==unit) ? 1.0f : 0.0f);
        }
    }

    void assignedToAll()
    {
        if (!_mtc && _mtc->getNumTextureWeights()>0) return;
        float div = 1.0f/static_cast<float>(_mtc->getNumTextureWeights());
        for(unsigned int i=0; i<_mtc->getNumTextureWeights(); ++i)
        {
            _mtc->setTextureWeight(i, div);
        }
    }

    osg::ref_ptr<osgTerrain::Terrain>           _terrain;
    osg::ref_ptr<osgFX::MultiTextureControl>    _mtc;
};

class CleanTechniqueReadFileCallback : public osgDB::ReadFileCallback
{

    public:

        class CleanTechniqueVisitor : public osg::NodeVisitor
        {
        public:
            CleanTechniqueVisitor():
                osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

            void apply(osg::Node& node)
            {
                osgTerrain::TerrainTile* tile = dynamic_cast<osgTerrain::TerrainTile*>(&node);
                if (tile)
                {
                    if (tile->getTerrainTechnique())
                    {
                        // OSG_NOTICE<<"Resetting TerrainTechnhique "<<tile->getTerrainTechnique()->className()<<" to 0"<<std::endl;
                        tile->setTerrainTechnique(0);
                    }
                }
                else
                {
                    traverse(node);
                }
            }
        };


        virtual osgDB::ReaderWriter::ReadResult readNode(const std::string& filename, const osgDB::Options* options)
        {
            osgDB::ReaderWriter::ReadResult rr = ReadFileCallback::readNode(filename, options);
            if (rr.validNode())
            {
                CleanTechniqueVisitor ctv;
                rr.getNode()->accept(ctv);
            }
            return rr;
        }

    protected:
        virtual ~CleanTechniqueReadFileCallback() {}
};


int main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc, argv);

    // construct the viewer.
    osgViewer::Viewer viewer(arguments);
	viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	viewer.getCamera()->setProjectionMatrixAsPerspective(40, 1, 1.0, 10000);
	osgTerrain::Terrain* mTerrain = new osgTerrain::Terrain;
    // set up the camera manipulators.
    {
        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

        keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
        keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
        keyswitchManipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator() );
        keyswitchManipulator->addMatrixManipulator( '4', "Terrain", new osgGA::TerrainManipulator() );

        std::string pathfile;
        char keyForAnimationPath = '5';
       

        viewer.setCameraManipulator( keyswitchManipulator.get() );
    }


    // add the state manipulator
    viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );

    // add the stats handler
    viewer.addEventHandler(new osgViewer::StatsHandler);
	const std::string base_name = R"(.\data\levels\Canyon\canyon.tif)";
	const std::string height_field_path = osgDB::getNameLessExtension(base_name) + "_height" + osgDB::getFileExtensionIncludingDot(base_name) + ".gdal";
	const std::string diff_field_path = osgDB::getNameLessExtension(base_name) + "_diff" + osgDB::getFileExtensionIncludingDot(base_name) + ".gdal";
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
	options->setBuildKdTreesHint(osgDB::Options::BUILD_KDTREES);


	osg::HeightField* height_field = osgDB::readHeightFieldFile(height_field_path, options);

	osg::ref_ptr<osgTerrain::DisplacementMappingTechnique> terrain_geometry_technique = new osgTerrain::DisplacementMappingTechnique();
	//terrain_geometry_technique->setFilterMatrixAs(osgTerrain::GeometryTechnique::GAUSSIAN);
	mTerrain->getOrCreateStateSet()->setDefine("LIGHTING", osg::StateAttribute::OVERRIDE);
	mTerrain->getOrCreateStateSet()->setDefine("HEIGHTFIELD_LAYER", osg::StateAttribute::OVERRIDE);

	if (!height_field)
	{
		throw (std::runtime_error("Cannot load specified height field: " + base_name));
	}

	//try to find a visual layer


	//create a locator to move/scale the data
	osg::ref_ptr<osgTerrain::Locator> Locator1 = new osgTerrain::Locator;
	Locator1->setCoordinateSystemType(osgTerrain::Locator::PROJECTED);
	Locator1->setTransformAsExtents(0.0, 0.0,2048, 2048);

	//create a height field layer from the height field, using the locator
	osg::Texture::FilterMode filter = osg::Texture::LINEAR;

	osg::ref_ptr<osgTerrain::HeightFieldLayer> hf_layer = new osgTerrain::HeightFieldLayer(height_field);
	hf_layer->setLocator(Locator1);
	hf_layer->setMagFilter(filter);
	hf_layer->transform(0, 0.01);
	//create a terrain-tile and add the height field
	osg::ref_ptr<osgTerrain::TerrainTile> terrain_tile = new osgTerrain::TerrainTile;
	terrain_tile->setElevationLayer(hf_layer.get());
	terrain_tile->setRequiresNormals(true);


	terrain_tile->setTerrainTechnique(terrain_geometry_technique);
	terrain_tile->setDirty(true);

	osg::Image* diffuse_texture = osgDB::readImageFile(diff_field_path);
	if (diffuse_texture)
	{
		terrain_tile->setColorLayer(0, new osgTerrain::ImageLayer(diffuse_texture));
	}

	mTerrain->addChild(terrain_tile.get());

	mTerrain->setTerrainTechniquePrototype(terrain_geometry_technique);

// 	osg::ref_ptr<osgGA::EventHandler> pick_handler = new nsGameCore::PickHandler(mrGameCore);
// 
// 	mrGameCore.getRenderCore().addEventHandler(pick_handler);

	//////////////////////////////////////////////////////////////////////////

	//terrain_geometry_technique->calculateAmbientApperture();

	//nsRenderer::ShaderProgram* program = nsRenderer::ShaderProgramHelpers::loadProgram("data/shaders/Terrain", nsRenderer::ShaderInfo());
	//mTerrain->g
    viewer.setSceneData(mTerrain );

    
    return viewer.run();

}
