/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxReconstructThreads.h"

#include "cxImage.h"
#include "cxReconstructPreprocessor.h"
#include "cxReconstructCore.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cx
{


ThreadedTimedReconstructPreprocessor::ThreadedTimedReconstructPreprocessor(PatientModelServicePtr patientModelService, ReconstructPreprocessorPtr input, std::vector<ReconstructCorePtr> cores) :
	cx::ThreadedTimedAlgorithm<void> ("US PreReconstruction", 30),
	mPatientModelService(patientModelService)
{
	mInput = input;
	mUseDefaultMessages = false;
	mCores = cores;
}

ThreadedTimedReconstructPreprocessor::~ThreadedTimedReconstructPreprocessor()
{
}

void ThreadedTimedReconstructPreprocessor::preProcessingSlot()
{
}

void ThreadedTimedReconstructPreprocessor::calculate()
{
	std::vector<bool> angio;
	for (unsigned i=0; i<mCores.size(); ++i)
		angio.push_back(mCores[i]->getInputParams().mAngio);

	std::vector<cx::ProcessedUSInputDataPtr> processedInput = mInput->createProcessedInput(angio);

	for (unsigned i=0; i<mCores.size(); ++i)
	{
		mCores[i]->initialize(processedInput[i], mInput->getOutputVolumeParams());
	}
}

void ThreadedTimedReconstructPreprocessor::postProcessingSlot()
{
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ThreadedTimedReconstructCore::ThreadedTimedReconstructCore(PatientModelServicePtr patientModelService, ViewServicePtr viewService, ReconstructCorePtr reconstructer) :
	cx::ThreadedTimedAlgorithm<void> ("US Reconstruction", 30),
	mPatientModelService(patientModelService),
	mViewService(viewService)
{
	mUseDefaultMessages = false;
	mReconstructer = reconstructer;
}

ThreadedTimedReconstructCore::~ThreadedTimedReconstructCore()
{
}

void ThreadedTimedReconstructCore::preProcessingSlot()
{
	mReconstructer->threadedPreReconstruct();
}

void ThreadedTimedReconstructCore::calculate()
{
	mReconstructer->threadedReconstruct();
}

void ThreadedTimedReconstructCore::postProcessingSlot()
{
	mReconstructer->threadedPostReconstruct();

	mPatientModelService->autoSave();
	mViewService->autoShowData(mReconstructer->getOutput());
}

}

