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

#include "catch.hpp"

#include "cxtestReconstructAlgorithmFixture.h"
#include "sscMessageManager.h"
//#include "cxOpenCLUtilities.h"
#include "cxOpenCLReconstruction.h"

namespace cxtest
{

TEST_CASE("OpenCLReconstruction","[unit][usreconstruction][OpenCL][OpenCLReconstruction]")
{
	cx::messageManager()->initialize();

	ReconstructAlgorithmFixture fixture;

	fixture.setOverallBoundsAndSpacing(100, 5);
	fixture.getInputGenerator()->setSpherePhantom();

	boost::shared_ptr<cx::OpenCLReconstruction> algorithm(new cx::OpenCLReconstruction);

	fixture.setAlgorithm(algorithm);


	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("OpenCLReconstruction");

	fixture.reconstruct(settings);

	cx::messageManager()->shutdown();

}
}//namespace cxtest
