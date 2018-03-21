/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxReconstructionExecuter.h"
#include "cxTimedAlgorithm.h"
#include "cxReconstructThreads.h"

#include "cxLogger.h"

namespace cx
{

cx::TimedAlgorithmPtr ReconstructionExecuter::getThread()
{
	return mPipeline;
}

void ReconstructionExecuter::startNonThreadedReconstruction(ReconstructionMethodService* algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio)
{
	cx::ReconstructPreprocessorPtr preprocessor = this->createPreprocessor(par, fileData);
	mCores = this->createCores(algo, par, createBModeWhenAngio);

	std::vector<bool> angio;
	for (unsigned i=0; i<mCores.size(); ++i)
		angio.push_back(mCores[i]->getInputParams().mAngio);

	std::vector<cx::ProcessedUSInputDataPtr> processedInput = preprocessor->createProcessedInput(angio);

	for (unsigned i=0; i<mCores.size(); ++i)
	{
		mCores[i]->initialize(processedInput[i], preprocessor->getOutputVolumeParams());
	}
	for (unsigned i=0; i<mCores.size(); ++i)
	{
		mCores[i]->reconstruct();
	}
}

void ReconstructionExecuter::startReconstruction(ReconstructionMethodService* algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio)
{
	if (mPipeline)
	{
		reportError("Reconstruct Executer can only be run once. Ignoring start.");
		return;
	}

	if (!fileData.isValid())
		return;

	//Don't create an extra B-Mode volume if input data is 8 bit
	if (fileData.is8bit())
		createBModeWhenAngio = false;

	mCores = this->createCores(algo, par, createBModeWhenAngio);
	if (mCores.empty())
		reportWarning("Failed to start reconstruction");

	cx::CompositeTimedAlgorithmPtr algorithm = this->assembleReconstructionPipeline(mCores, par, fileData);
	this->launch(algorithm);
}

std::vector<cx::ImagePtr> ReconstructionExecuter::getResult()
{
	std::vector<cx::ImagePtr> retval;
	if (mPipeline && !mPipeline->isFinished())
		return retval;

	for (unsigned i=0; i<mCores.size(); ++i)
		retval.push_back(mCores[i]->getOutput());

	return retval;
}

void ReconstructionExecuter::launch(cx::TimedAlgorithmPtr thread)
{
	mPipeline = thread;
	emit reconstructAboutToStart();
	connect(thread.get(), SIGNAL(finished()), this, SIGNAL(reconstructFinished()));
	thread->execute();
	emit reconstructStarted();
}

cx::CompositeTimedAlgorithmPtr ReconstructionExecuter::assembleReconstructionPipeline(std::vector<ReconstructCorePtr> cores, ReconstructCore::InputParams par, USReconstructInputData fileData)
{
	cx::CompositeSerialTimedAlgorithmPtr pipeline(new cx::CompositeSerialTimedAlgorithm("US Reconstruction"));

	ReconstructPreprocessorPtr preprocessor = this->createPreprocessor(par, fileData);
	pipeline->append(ThreadedTimedReconstructPreprocessor::create(mPatientModelService, preprocessor, cores));

	cx::CompositeTimedAlgorithmPtr temp = pipeline;
	if(this->canCoresRunInParallel(cores) && cores.size()>1)
	{
		cx::CompositeParallelTimedAlgorithmPtr parallel(new cx::CompositeParallelTimedAlgorithm());
		pipeline->append(parallel);
		temp = parallel;
		reportDebug("Running reconstruction cores in parallel.");
	}

	for (unsigned i=0; i<cores.size(); ++i)
		temp->append(ThreadedTimedReconstructCore::create(mPatientModelService, mViewService, cores[i]));

	return pipeline;
}

bool ReconstructionExecuter::canCoresRunInParallel(std::vector<ReconstructCorePtr> cores)
{
	bool parallelizable = true;

	std::vector<ReconstructCorePtr>::iterator it;
	for(it = cores.begin(); it != cores.end(); ++it)
		parallelizable = parallelizable && (it->get()->getInputParams().mAlgorithmUid == "pnn");

	return parallelizable;
}

ReconstructPreprocessorPtr ReconstructionExecuter::createPreprocessor(ReconstructCore::InputParams par, USReconstructInputData fileData)
{
	ReconstructPreprocessorPtr retval(new ReconstructPreprocessor(mPatientModelService));
	retval->initialize(par, fileData);

	return retval;
}

std::vector<ReconstructCorePtr> ReconstructionExecuter::createCores(ReconstructionMethodService* algo, ReconstructCore::InputParams par, bool createBModeWhenAngio)
{
	std::vector<ReconstructCorePtr> retval;

	if (createBModeWhenAngio && par.mAngio)
	{
		ReconstructCorePtr core = this->createBModeCore(par, algo);
		if (core)
			retval.push_back(core);
		core = this->createCore(par, algo);
		if (core)
			retval.push_back(core);
	}
	// only one thread
	else
	{
		ReconstructCorePtr core = this->createCore(par, algo);
		if (core)
			retval.push_back(core);
	}

	return retval;
}

ReconstructCorePtr ReconstructionExecuter::createCore(ReconstructCore::InputParams par, ReconstructionMethodService* algo)
{
	ReconstructCorePtr retval(new ReconstructCore(mPatientModelService));
	retval->initialize(par, algo);
	return retval;
}

ReconstructCorePtr ReconstructionExecuter::createBModeCore(ReconstructCore::InputParams par, ReconstructionMethodService* algo)
{
	ReconstructCorePtr retval(new ReconstructCore(mPatientModelService));
	par.mAngio = false;
	par.mTransferFunctionPreset = "US B-Mode";
	retval->initialize(par, algo);
	return retval;
}



} /* namespace cx */
