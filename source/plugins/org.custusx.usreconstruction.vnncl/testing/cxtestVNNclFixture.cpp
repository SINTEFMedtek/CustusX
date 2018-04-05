/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxtestVNNclFixture.h"

#include "catch.hpp"

#include "cxVNNclAlgorithm.h"
#include "cxVNNclReconstructionMethodService.h"
#include "cxBoolProperty.h"

#include "cxLogicManager.h"
#include "cxReporter.h"
#include "cxtestUtilities.h"

namespace cxtest
{

VNNclFixture::VNNclFixture() :
	mAlgorithm(NULL)
{
	cx::UsReconstructionServicePtr reconstructer = mFixture.getManager();

	reconstructer->selectData(mRealData.getSourceFilename());
	reconstructer->getParam("Algorithm")->setValueFromVariant("vnn_cl");
	reconstructer->getParam("Angio data")->setValueFromVariant(false);
	reconstructer->getParam("Dual Angio")->setValueFromVariant(false);

	cx::ReconstructionMethodService* algorithmService = reconstructer->createAlgorithm();
	REQUIRE(algorithmService);
	mAlgorithm = dynamic_cast<cx::VNNclReconstructionMethodService*>(algorithmService);
	REQUIRE(mAlgorithm);// Check if we got the algorithm

	mAlgo = reconstructer->getSettings().getElement("algorithms", "vnn_cl");
	mAlgorithm->getRadiusOption(mAlgo)->setValue(1.0);

	this->initVNN();
}

VNNclFixture::~VNNclFixture()
{
	delete mAlgorithm;
	mAlgorithm = NULL;
}

void VNNclFixture::initVNN()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("VNN");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(1);
	mAlgorithm->getNStartsOption(mAlgo)->setValue(1);
}

void VNNclFixture::initVNN2()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("VNN2");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(8);
}

void VNNclFixture::initDW()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("DW");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(8);
}

void VNNclFixture::initAnisotropic()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("Anisotropic");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(8);
}

void VNNclFixture::initVNNMultistart()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("VNN");
	mAlgorithm->getNStartsOption(mAlgo)->setValue(5);
}

void VNNclFixture::initVNNClosest()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("VNN");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Closest");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(8);
}

void VNNclFixture::reconstruct()
{
	mFixture.reconstruct();
}

void VNNclFixture::verify()
{
	REQUIRE(mFixture.getOutput().size()==1);
	mRealData.validateBModeData(mFixture.getOutput()[0]);
}

/////////////////////////////////

VNNclSyntheticFixture::VNNclSyntheticFixture() :
	mAlgorithm(NULL)
{
	cx::LogicManager::initialize();
	cx::Reporter::initialize();

	ctkPluginContext* pluginContext = cx::logicManager()->getPluginContext();
	mAlgorithm = new cx::VNNclReconstructionMethodService(pluginContext);

	mFixture.setOverallBoundsAndSpacing(100, 5);
	mFixture.getInputGenerator()->setSpherePhantom();

	mAlgorithm->enableProfiling();

	mSettings = mDomdoc.createElement("vnn_cl");

	this->initVNN();
}

VNNclSyntheticFixture::~VNNclSyntheticFixture()
{
	delete mAlgorithm;
	mAlgorithm = NULL;
	//need to be sure opencl thread is finished before shutting down Reporter,
	//or else we could get seg fault because og a callbackk from opencl to Reporter after it is shut down
	Utilities::sleep_sec(1);
	cx::Reporter::shutdown();
	cx::LogicManager::shutdown();
}

void VNNclSyntheticFixture::initVNN()
{
	mMethodName = "VNN";
	mAlgorithm->getRadiusOption(mSettings)->setValue(10);
	mAlgorithm->getMethodOption(mSettings)->setValue("VNN");
	mAlgorithm->getPlaneMethodOption(mSettings)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mSettings)->setValue(8);
	mAlgorithm->getNStartsOption(mSettings)->setValue(1);
}

void VNNclSyntheticFixture::initVNN2()
{
	mMethodName = "VNN2";
	mAlgorithm->getMethodOption(mSettings)->setValue("VNN2");
	mAlgorithm->getPlaneMethodOption(mSettings)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mSettings)->setValue(8);
	mAlgorithm->getNStartsOption(mSettings)->setValue(1);
}

void VNNclSyntheticFixture::initDW()
{
	mMethodName = "DW";
	mAlgorithm->getMethodOption(mSettings)->setValue("DW");
	mAlgorithm->getPlaneMethodOption(mSettings)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mSettings)->setValue(8);
	mAlgorithm->getNStartsOption(mSettings)->setValue(1);
}

void VNNclSyntheticFixture::initAnisotropic()
{
	mMethodName = "Anisotropic";
	mAlgorithm->getMethodOption(mSettings)->setValue("Anisotropic");
	mAlgorithm->getPlaneMethodOption(mSettings)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mSettings)->setValue(8);
	mAlgorithm->getNStartsOption(mSettings)->setValue(1);
	mAlgorithm->getBrightnessWeightOption(mSettings)->setValue(0);
	mAlgorithm->getNewnessWeightOption(mSettings)->setValue(0);
}

void VNNclSyntheticFixture::initVNNMultistart()
{
	mMethodName = "Multistart search";
	mAlgorithm->getMethodOption(mSettings)->setValue("VNN");
	mAlgorithm->getPlaneMethodOption(mSettings)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mSettings)->setValue(8);
	mAlgorithm->getNStartsOption(mSettings)->setValue(5);
}

void VNNclSyntheticFixture::reconstruct()
{
	mFixture.setAlgorithm(mAlgorithm);
	mFixture.reconstruct(mSettings);

	double executionTime = mAlgorithm->getKernelExecutionTime();
	JenkinsMeasurement jenkins;
	jenkins.createOutput(mMethodName, QString::number(executionTime));
}

void VNNclSyntheticFixture::verify()
{
	mFixture.checkRMSBelow(20.0);
	mFixture.checkCentroidDifferenceBelow(1);
	mFixture.checkMassDifferenceBelow(0.01);

}

}//namespace
