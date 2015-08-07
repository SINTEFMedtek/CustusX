/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	QDomElement settings = domdoc.createElement("PNN");

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


