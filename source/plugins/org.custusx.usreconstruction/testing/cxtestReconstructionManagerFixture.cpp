/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestReconstructionManagerFixture.h"

#include "catch.hpp"
#include <QApplication>
#include "cxDoubleProperty.h"
#include "cxTimedAlgorithm.h"

#include "cxBoolProperty.h"

#include "cxDataLocations.h"
#include "cxReconstructPreprocessor.h"
#include "cxReconstructionExecuter.h"
#include "cxLogicManager.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxUsReconstructionServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxSessionStorageService.h"
#include "cxFileManagerServiceProxy.h"


namespace cxtest
{

ReconstructionManagerTestFixture::ReconstructionManagerTestFixture() :
	mViewService(cx::ViewService::getNullObject())
{
	mVerbose = false;
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	QString folder = cx::DataLocations::getTestDataPath() + "/temp/test.cx3";
	cx::logicManager()->getSessionStorageService()->load(folder);

	ctkPluginContext *pluginContext = cx::logicManager()->getPluginContext();
	mPatientModelService = cx::PatientModelServiceProxy::create(pluginContext);
	mFileManagerService = cx::FileManagerServiceProxy::create(pluginContext);
}

ReconstructionManagerTestFixture::~ReconstructionManagerTestFixture()
{
	cx::LogicManager::shutdown();
}

void ReconstructionManagerTestFixture::setPNN_InterpolationSteps(int value)
{
	cx::UsReconstructionServicePtr manager = this->getManager();
	QDomElement algo = manager->getSettings().getElement("algorithms", "pnn");
	cx::ReconstructionMethodService* algorithm = manager->createAlgorithm();
	REQUIRE(algorithm);

	std::vector<cx::PropertyPtr> adaptors = algorithm->getSettings(algo);
	cx::PropertyPtr adapter = cx::Property::findProperty(adaptors, "interpolationSteps");
	if(adapter)
		adapter->setValueFromVariant(value);
	else
		cx::reportError("Could not find adapter interpolationSteps");
}

cx::FileManagerServicePtr ReconstructionManagerTestFixture::getFileManagerService()
{
	return mFileManagerService;
}

void ReconstructionManagerTestFixture::reconstruct()
{
	mOutput.clear();
	cx::UsReconstructionServicePtr reconstructer = this->getManager();
	bool createBModeWhenAngio = reconstructer->getParam("Dual Angio")->getValueAsVariant().toBool();

	cx::ReconstructionExecuterPtr executer(new cx::ReconstructionExecuter(mPatientModelService, mViewService));

	bool success = executer->startNonThreadedReconstruction(reconstructer->createAlgorithm(),
			reconstructer->createCoreParameters(),
			reconstructer->getSelectedFileData(),
			createBModeWhenAngio);
	REQUIRE(success);

	mOutput = executer->getResult();
}

void ReconstructionManagerTestFixture::threadedReconstruct()
{
	mOutput.clear();
	cx::UsReconstructionServicePtr reconstructer = this->getManager();

	cx::ReconstructionExecuterPtr executer(new cx::ReconstructionExecuter(mPatientModelService, mViewService));

	bool createBModeWhenAngio = reconstructer->getParam("Dual Angio")->getValueAsVariant().toBool();
	bool success = executer->startReconstruction(reconstructer->createAlgorithm(),
			reconstructer->createCoreParameters(),
			reconstructer->getSelectedFileData(),
			createBModeWhenAngio);
	REQUIRE(success);
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

cx::UsReconstructionServicePtr ReconstructionManagerTestFixture::getManager()
{
	if (!mManager)
	{
//		cx::XmlOptionFile settings;
//		cx::UsReconstructionServicePtr reconstructer(new cx::ReconstructionManager(settings,""));

		//TODO: mocking UsReconstructionService with null object is probably not enough
//		cx::UsReconstructionServicePtr reconstructer(cx::UsReconstructionService::getNullObject());


		ctkPluginContext *pluginContext = cx::logicManager()->getPluginContext();
		cx::UsReconstructionServicePtr reconstructer = cx::UsReconstructionServicePtr(new cx::UsReconstructionServiceProxy(pluginContext)); //Can't mock


//		reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
//		reconstructer->setOutputRelativePath("Images");

		mManager = reconstructer;
	}
	return mManager;
}

} // namespace cxtest


