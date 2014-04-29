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

#include "cxReconstructThreads.h"

#include "cxImage.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxViewManager.h"
#include "cxReconstructPreprocessor.h"
#include "cxReconstructCore.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cx
{



ThreadedTimedReconstructPreprocessor::ThreadedTimedReconstructPreprocessor(ReconstructPreprocessorPtr input, std::vector<ReconstructCorePtr> cores) :
	cx::ThreadedTimedAlgorithm<void> ("US PreReconstruction", 30)
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
	mInput->initializeCores(mCores);
}

void ThreadedTimedReconstructPreprocessor::postProcessingSlot()
{
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ThreadedTimedReconstructCore::ThreadedTimedReconstructCore(ReconstructCorePtr reconstructer) :
	cx::ThreadedTimedAlgorithm<void> ("US Reconstruction", 30)
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

	cx::patientService()->getPatientData()->autoSave();
	if (cx::viewManager()) // might be called by auto test - no service
		cx::viewManager()->autoShowData(mReconstructer->getOutput());
}

}

