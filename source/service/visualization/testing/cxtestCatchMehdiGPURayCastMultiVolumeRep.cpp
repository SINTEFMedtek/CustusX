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

#include "cxConfig.h"
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
#include "catch.hpp"
#include "cxMehdiGPURayCastMultiVolumeRep.h"

#include "cxtestMultiVolume3DRepProducerFixture.h"
//#include "cxTestCustusXController.h"

namespace cxtest
{

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 3 small volumes.", "[integration][gui]")
{
	MultiVolume3DRepProducerFixture fixture;

	unsigned imageCount = 3;
	fixture.initializeVisualizerAndImages("vtkOpenGLGPUMultiVolumeRayCastMapper", imageCount);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = fixture.downcastRep<cx::MehdiGPURayCastMultiVolumeRep>(0);
	REQUIRE(rep);

/*
	cx::DataLocations::setTestMode();
	CustusXController custusX(NULL);
	custusX.start();
	qApp->exec();
	custusX.stop();*/

}


} // namespace cxtest


#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
