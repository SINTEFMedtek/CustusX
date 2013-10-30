#ifdef CX_USE_LEVEL_SET
#include "catch.hpp"
#include "cxLevelSetFilter.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscToolManager.h"
#include "cxToolManager.h"
#include "sscManualTool.h"
#include "sscData.h"
#include "sscImage.h"
#include "cxtestUtilities.h"

namespace cxtest {

TEST_CASE("LevelSetFilter: getSeedPointFromTool", "[unit][plugins][Algorithm][LevelSetFilter]") 
{
    cx::cxToolManager::initializeObject();
    cx::cxToolManager * toolmanager = cx::cxToolManager::getInstance();
    cx::ManualToolPtr tool = toolmanager->getManualTool();
    cx::Vector3D toolTipPoint;
    toolTipPoint.setRandom();
    tool->set_prMt(cx::createTransformTranslate(toolTipPoint));

    cx::DataPtr data;
    cx::Vector3D point = cx::LevelSetFilter::getSeedPointFromTool(data);
    REQUIRE(toolTipPoint(0) == point(0));
    REQUIRE(toolTipPoint(1) == point(1));
    REQUIRE(toolTipPoint(2) == point(2));

}

TEST_CASE("LevelSetFilter: isSeedPointInsideImage", "[unit][plugins][Algorithm][LevelSetFilter]")
{
    cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(10,10,10), 1);
    cx::Vector3D point;
    point.setOnes();

    CHECK(cx::LevelSetFilter::isSeedPointInsideImage(point, image));

    point(1) = 12;
    CHECK_FALSE(cx::LevelSetFilter::isSeedPointInsideImage(point, image));
}

}; // end cxtest namespace
#endif // end ifdef CX_USE_LEVEL_SET
