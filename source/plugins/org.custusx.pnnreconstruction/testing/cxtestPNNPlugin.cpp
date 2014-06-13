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
#include <QDomElement>
#include "cxPNNReconstructionService.h"
#include "cxDummyTool.h"
#include "cxReporter.h"
#include "cxtestReconstructionAlgorithmFixture.h"
#include "cxtestUtilities.h"

namespace cxtest
{

TEST_CASE("ReconstructAlgorithm: PNN on sphere","[unit][usreconstruction][synthetic][pnn]")
{
	ReconstructionAlgorithmFixture fixture;
	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("PNN");

	fixture.setOverallBoundsAndSpacing(100, 5);
	fixture.setVerbose(true);
	fixture.getInputGenerator()->setSpherePhantom();

	fixture.setAlgorithm(cx::PNNReconstructionService::create());
	fixture.reconstruct(settings);

	fixture.checkRMSBelow(20.0);
	fixture.checkCentroidDifferenceBelow(1);
	fixture.checkMassDifferenceBelow(0.01);

	if (fixture.getVerbose())
	{
		fixture.saveOutputToFile("test/sphere_rec.mhd");
		fixture.saveNominalOutputToFile("test/sphere_nom.mhd");
	}
}

TEST_CASE("ReconstructAlgorithm: PNN on sphere, tilt","[unit][usreconstruction][synthetic][pnn]")
{
	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("PNN");

	ReconstructionAlgorithmFixture fixture;
	fixture.setVerbose(false);

	SyntheticReconstructInputPtr generator = fixture.getInputGenerator();
	generator->defineProbeMovementSteps(40);
	generator->defineProbeMovementNormalizedTranslationRange(0.8);
	generator->defineProbeMovementAngleRange(M_PI/6);
	generator->defineProbe(cx::DummyToolTestUtilities::createProbeDataLinear(100, 100, Eigen::Array2i(150,150)));
	generator->setSpherePhantom();
	fixture.defineOutputVolume(100, 2);

	fixture.setAlgorithm(cx::PNNReconstructionService::create());
	fixture.reconstruct(settings);

	fixture.checkRMSBelow(30.0);
	fixture.checkCentroidDifferenceBelow(2);
	fixture.checkMassDifferenceBelow(0.01);

	if (fixture.getVerbose())
	{
		fixture.saveOutputToFile("test/sphere_rec.mhd");
		fixture.saveNominalOutputToFile("test/sphere_nom.mhd");
	}
}

} // namespace cxtest


