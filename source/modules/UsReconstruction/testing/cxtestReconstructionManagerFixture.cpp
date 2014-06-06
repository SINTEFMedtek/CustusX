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

#include "cxtestReconstructionManagerFixture.h"

#include "cxDataManager.h"
#include "catch.hpp"
#include "cxPNNReconstructAlgorithm.h"
#include <QApplication>
#include "cxDoubleDataAdapterXml.h"
#include "cxTimedAlgorithm.h"

#include "cxBoolDataAdapterXml.h"

#include "cxDataLocations.h"
#include "cxReconstructPreprocessor.h"
#include "cxReconstructionExecuter.h"
#include "cxLogicManager.h"

namespace cxtest
{

ReconstructionManagerTestFixture::ReconstructionManagerTestFixture()
{
	mVerbose = false;
	cx::LogicManager::initialize();
}

ReconstructionManagerTestFixture::~ReconstructionManagerTestFixture()
{
	cx::LogicManager::shutdown();
}

void ReconstructionManagerTestFixture::setPNN_InterpolationSteps(int value)
{
	cx::ReconstructionManagerPtr manager = this->getManager();
	// set an algorithm-specific parameter
	QDomElement algo = manager->getSettings().getElement("algorithms", "PNN");
	boost::shared_ptr<cx::PNNReconstructAlgorithm> algorithm;
	algorithm = boost::dynamic_pointer_cast<cx::PNNReconstructAlgorithm>(manager->createAlgorithm());
	REQUIRE(algorithm);// Check if we got the PNN algorithm

	algorithm->getInterpolationStepsOption(algo)->setValue(value);
}

void ReconstructionManagerTestFixture::reconstruct()
{
	mOutput.clear();
	cx::ReconstructionManagerPtr reconstructer = this->getManager();
	bool createBModeWhenAngio = reconstructer->getParams()->mCreateBModeWhenAngio->getValue();

	cx::ReconstructionExecuterPtr executer(new cx::ReconstructionExecuter());

	executer->startNonThreadedReconstruction(reconstructer->createAlgorithm(),
			reconstructer->createCoreParameters(),
			reconstructer->getSelectedFileData(),
			createBModeWhenAngio);

	mOutput = executer->getResult();
}

void ReconstructionManagerTestFixture::threadedReconstruct()
{
	mOutput.clear();
	cx::ReconstructionManagerPtr reconstructer = this->getManager();

	cx::ReconstructionExecuterPtr executer(new cx::ReconstructionExecuter());

	bool createBModeWhenAngio = reconstructer->getParams()->mCreateBModeWhenAngio->getValue();
	executer->startReconstruction(reconstructer->createAlgorithm(),
			reconstructer->createCoreParameters(),
			reconstructer->getSelectedFileData(),
			createBModeWhenAngio);
	cx::TimedAlgorithmPtr thread = executer->getThread();

	QObject::connect(thread.get(), SIGNAL(finished()), qApp, SLOT(quit()));
	qApp->exec();

	REQUIRE(thread->isFinished());
	REQUIRE(!thread->isRunning());

	mOutput = executer->getResult();
}

std::vector<cx::ImagePtr> ReconstructionManagerTestFixture::getOutput()
{
	return mOutput;
}

SyntheticVolumeComparerPtr ReconstructionManagerTestFixture::getComparerForOutput(SyntheticReconstructInputPtr input, int index)
{
	SyntheticVolumeComparerPtr comparer(new SyntheticVolumeComparer());
	comparer->setVerbose(this->getVerbose());
	comparer->setPhantom(input->getPhantom());
	comparer->setTestImage(this->getOutput()[index]);
	return comparer;
}

cx::ReconstructionManagerPtr ReconstructionManagerTestFixture::getManager()
{
	if (!mManager)
	{
		cx::XmlOptionFile settings;
		cx::ReconstructionManagerPtr reconstructer(new cx::ReconstructionManager(settings,""));

		reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
		reconstructer->setOutputRelativePath("Images");

		mManager = reconstructer;
	}
	return mManager;
}

} // namespace cxtest


