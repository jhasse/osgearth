/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2015 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include <osgViewer/Viewer>
#include <osgEarth/Notify>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/ExampleResources>

#include <osgEarth/Units>
#include <osgEarth/Viewpoint>

#define LC "[viewer] "

using namespace osgEarth;
using namespace osgEarth::Util;

int
usage(const char* name)
{
    OE_NOTICE 
        << "\nUsage: " << name << " file.earth" << std::endl
        << MapNodeHelper().usage() << std::endl;

    return 0;
}

int
main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc,argv);

    // help?
    if ( arguments.read("--help") )
        return usage(argv[0]);

    float vfov = -1.0f;
    arguments.read("--vfov", vfov);

    // create a viewer:
    osgViewer::Viewer viewer(arguments);
    viewer.setLightingMode( osg::View::NO_LIGHT );

    // Tell the database pager to not modify the unref settings
    viewer.getDatabasePager()->setUnrefImageDataAfterApplyPolicy( false, false );

    osgDB::Registry::instance()->getObjectWrapperManager()->findWrapper("osg::Image");

    // install our default manipulator (do this before calling load)
    viewer.setCameraManipulator( new EarthManipulator(arguments) );

    // disable the small-feature culling
    viewer.getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

    // configure the near/far so we don't clip things that are up close
    viewer.getCamera()->setNearFarRatio(0.00002);

    if ( vfov > 0.0 )
    {
        double fov, ar, n, f;
        viewer.getCamera()->getProjectionMatrixAsPerspective(fov, ar, n, f);
        viewer.getCamera()->setProjectionMatrixAsPerspective(vfov, ar, n, f);
    }

    // load an earth file, and support all or our example command-line options
    // and earth file <external> tags    
    osg::Node* node = MapNodeHelper().load( arguments, &viewer );
    if ( node )
    {
        viewer.setSceneData( node );

        return viewer.run();
    }
    else
    {
        return usage(argv[0]);
    }
}
