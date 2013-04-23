#include "testing/cxTestTSF.h"

#ifdef CX_USE_TSF
#include "parameters.hpp"
#include "tsf-config.h"
#endif //CX_USE_TSF

#include "cxDataLocations.h"

namespace cx {
void TestTSF::setUp()
{
	mParametersFolderPath = std::string(PARAMETERS_DIR);
}

void TestTSF::tearDown()
{
}

void TestTSF::testConstructor()
{}

void TestTSF::testInitParameters()
{
	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Initializing default parameters failed.", initParameters(mParametersFolderPath));
}

void TestTSF::testLoadPreset()
{
	std::string path = std::string(PARAMETERS_DIR);
	paramList neuroVesselsUSAParameters = initParameters(path);
	setParameter(neuroVesselsUSAParameters, "parameters", "Neuro-Vessels-USA");
	loadParameterPreset(neuroVesselsUSAParameters, path);

	//TODO
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "minimum") == 50);
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "maximum") == 200);
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "fmax") == 0.1);
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "min-mean-tdf") == 0.5);
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "radius-min") == 1.5);
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "radius-max") == 7.0);
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "small-blur") == 2.0);
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "large-blur") == 3.0);
//	CPPUNIT_ASSERT(getParamStr(neuroVesselsUSAParameters, "cropping") == "threshold");
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "cropping-threshold") == 50);
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "min-tree-length") == 10);
//	CPPUNIT_ASSERT(getParamBool(neuroVesselsUSAParameters, "sphere-segmentation") == true);
//	CPPUNIT_ASSERT(getParam(neuroVesselsUSAParameters, "cube-size") == 4);

}
} /* namespace cx */
