/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <QDomElement>
#include "cxPNNReconstructionMethodService.h"
#include "cxDummyTool.h"

#include "cxtestReconstructionAlgorithmFixture.h"
#include "cxtestUtilities.h"
#include "cxLogicManager.h"

namespace cxtest
{

TEST_CASE("ReconstructAlgorithm: PNN on sphere","[unit][usreconstruction][synthetic][pnn]")
{
	cx::LogicManager::initialize();
	ctkPluginContext* pluginContext = cx::logicManager()->getPluginContext();

	ReconstructionAlgorithmFixture fixture;
	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("pnn");

	fixture.setOverallBoundsAndSpacing(100, 5);
	fixture.setVerbose(true);
	fixture.getInputGenerator()->setSpherePhantom();

	fixture.setAlgorithm(new cx::PNNReconstructionMethodService(pluginContext));
	fixture.reconstruct(settings);

	fixture.checkRMSBelow(20.0);
	fixture.checkCentroidDifferenceBelow(1);
	fixture.checkMassDifferenceBelow(0.01);

	if (fixture.getVerbose())
	{
		fixture.saveOutputToFile("test/sphere_rec.mhd");
		fixture.saveNominalOutputToFile("test/sphere_nom.mhd");
	}
	cx::LogicManager::shutdown();
}

TEST_CASE("ReconstructAlgorithm: PNN on sphere, tilt","[unit][usreconstruction][synthetic][pnn]")
{
	cx::LogicManager::initialize();
	ctkPluginContext* pluginContext = cx::logicManager()->getPluginContext();

	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("pnn");

	ReconstructionAlgorithmFixture fixture;
	fixture.setVerbose(false);

	SyntheticReconstructInputPtr generator = fixture.getInputGenerator();
	generator->defineProbeMovementSteps(40);
	generator->defineProbeMovementNormalizedTranslationRange(0.8);
	generator->defineProbeMovementAngleRange(M_PI/6);
	generator->defineProbe(cx::DummyToolTestUtilities::createProbeDefinitionLinear(100, 100, Eigen::Array2i(150,150)));
	generator->setSpherePhantom();
	fixture.defineOutputVolume(100, 2);

	fixture.setAlgorithm(new cx::PNNReconstructionMethodService(pluginContext));
	fixture.reconstruct(settings);

	fixture.checkRMSBelow(30.0);
	fixture.checkCentroidDifferenceBelow(2);
	fixture.checkMassDifferenceBelow(0.01);

	if (fixture.getVerbose())
	{
		fixture.saveOutputToFile("test/sphere_rec.mhd");
		fixture.saveNominalOutputToFile("test/sphere_nom.mhd");
	}
	cx::LogicManager::shutdown();
}

} // namespace cxtest


