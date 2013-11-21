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

#include <vtkImageData.h>
#include <QDomElement>

#include "sscReconstructAlgorithm.h"
#include "TordReconstruct/cxSimpleSyntheticVolume.h"
#include "catch.hpp"
#include "sscPNNReconstructAlgorithm.h"

#include "cxtestReconstructAlgorithmFixture.h"

namespace cxtest
{

//TEST_CASE("ReconstructAlgorithm: cross not reconstructed fails","[usreconstruction][synthetic][hide][ca_rec0]")
//{
//	ReconstructAlgorithmFixture fixture;
//	fixture.setBoxAndLinesPhantom();
//	fixture.generateInput();
//	fixture.generateOutputVolume();

//	double zeroRMS = fixture.getRMS();
//	CHECK(zeroRMS > 30.0);
//}

TEST_CASE("ReconstructAlgorithm: PNN on cross","[usreconstruction][synthetic][hide][ca_rec1]")
{
	ReconstructAlgorithmFixture fixture;
	fixture.setBoxAndLinesPhantom();
	fixture.generateOutputVolume();
	fixture.generateInput();

	fixture.setAlgorithm(new cx::PNNReconstructAlgorithm());
	fixture.reconstruct();

	fixture.checkRMSBelow(15.0);
}



} // namespace cxtest


