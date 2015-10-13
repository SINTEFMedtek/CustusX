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

