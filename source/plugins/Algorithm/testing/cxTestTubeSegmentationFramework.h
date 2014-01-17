// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXTESTTUBESEGMENTATIONFRAMEWORK_H_
#define CXTESTTUBESEGMENTATIONFRAMEWORK_H_

#ifdef CX_USE_TSF
class QString;
class paramList;

class TestTubeSegmentationFramework
{
public:
	TestTubeSegmentationFramework();
	~TestTubeSegmentationFramework();
	void setUp();
	void tearDown();

	void testConstructor();///< Test constructor

	void testParameters(); ///< Test the default parameters read from file (TSF)
	void testDefaultPreset();
	void testSyntheticVascusynthPreset();
	void testPhantomAccUSPreset();
	void testNeuroVesselsMRAPreset();
	void testNeuroVesselsUSAPreset();
	void testLungAirwaysCTPreset();
	void testLiverVesselsCTPreset();
	void testLiverVesselsMRPreset();

	void testLoadParameterFile(); ///< Test to see if the options(adapters) are correctly fill with information form the parameter file

private:
	paramList loadPreset(QString preset);
	void runFilter(QString preset);
};
#endif //CX_USE_TSF
#endif /* CXTESTTUBESEGMENTATIONFRAMEWORK_H_ */
