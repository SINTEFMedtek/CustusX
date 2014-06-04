#include <cxReconstructionExecuter.h>
#include "cxTimedAlgorithm.h"
#include "cxReconstructThreads.h"


namespace cx
{

std::set<cx::TimedAlgorithmPtr> ReconstructionExecuter::getThreadedReconstruction()
{
	return mThreadedReconstruction;
}

std::vector<ReconstructCorePtr> ReconstructionExecuter::startReconstruction(ReconstructionServicePtr algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio, bool validInputData)
{
	std::vector<ReconstructCorePtr> cores = this->createCores(algo, par, createBModeWhenAngio, validInputData);

	if (cores.empty())
	{
		reportWarning("Failed to start reconstruction");
		return cores;
	}
	cx::CompositeTimedAlgorithmPtr algorithm = this->assembleReconstructionPipeline(cores, par, fileData, validInputData);

	this->launch(algorithm);

	return cores;
}

void ReconstructionExecuter::launch(cx::TimedAlgorithmPtr thread)
{
	mThreadedReconstruction.insert(thread);
	emit reconstructAboutToStart();
	connect(thread.get(), SIGNAL(finished()), this, SLOT(threadFinishedSlot())); // connect after emit, to allow listeners to get thread at finish
	thread->execute();
}

void ReconstructionExecuter::threadFinishedSlot()
{
	std::set<cx::TimedAlgorithmPtr>::iterator iter;
	for(iter=mThreadedReconstruction.begin(); iter!=mThreadedReconstruction.end(); )
	{
		if ((*iter)->isFinished())
		{
			mThreadedReconstruction.erase(iter);
			iter = mThreadedReconstruction.begin();
		}
		else
			++iter;
	}

	if (mThreadedReconstruction.empty())
		emit reconstructFinished();

//	if (mThreadedReconstruction.empty())
//		mOriginalFileData.mUsRaw->purgeAll();
}

cx::CompositeTimedAlgorithmPtr ReconstructionExecuter::assembleReconstructionPipeline(std::vector<ReconstructCorePtr> cores, ReconstructCore::InputParams par, USReconstructInputData fileData, bool validInputData)
{
	cx::CompositeSerialTimedAlgorithmPtr pipeline(new cx::CompositeSerialTimedAlgorithm("US Reconstruction"));

	ReconstructPreprocessorPtr preprocessor = this->createPreprocessor(par, fileData, validInputData);
	pipeline->append(ThreadedTimedReconstructPreprocessor::create(preprocessor, cores));

	cx::CompositeTimedAlgorithmPtr temp = pipeline;
	if(this->canCoresRunInParallel(cores))
	{
		cx::CompositeParallelTimedAlgorithmPtr parallel(new cx::CompositeParallelTimedAlgorithm());
		pipeline->append(parallel);
		temp = parallel;
		reportDebug("Running reconstruction cores in parallel.");
	}

	for (unsigned i=0; i<cores.size(); ++i)
		temp->append(ThreadedTimedReconstructCore::create(cores[i]));

	return pipeline;
}

bool ReconstructionExecuter::canCoresRunInParallel(std::vector<ReconstructCorePtr> cores)
{
	bool parallelizable = true;

	std::vector<ReconstructCorePtr>::iterator it;
	for(it = cores.begin(); it != cores.end(); ++it)
		parallelizable = parallelizable && (it->get()->getInputParams().mAlgorithmUid == "PNN");

	return parallelizable;
}

ReconstructPreprocessorPtr ReconstructionExecuter::createPreprocessor(ReconstructCore::InputParams par, USReconstructInputData fileData, bool validInputData)
{
	if (!validInputData)
		return ReconstructPreprocessorPtr();

	ReconstructPreprocessorPtr retval(new ReconstructPreprocessor());

//	ReconstructCore::InputParams par = this->createCoreParameters();

//	USReconstructInputData fileData = mOriginalFileData;
//	fileData.mUsRaw = mOriginalFileData.mUsRaw->copy();

	retval->initialize(par, fileData);

	return retval;
}

std::vector<ReconstructCorePtr> ReconstructionExecuter::createCores(ReconstructionServicePtr algo, ReconstructCore::InputParams par, bool createBModeWhenAngio, bool validInputData)
{
	std::vector<ReconstructCorePtr> retval;

//	par.mAngio = mParams->mAngioAdapter->getValue();
	// create both
//	if (mParams->mCreateBModeWhenAngio->getValue() && mParams->mAngioAdapter->getValue())
	if (createBModeWhenAngio && par.mAngio)
	{
		ReconstructCorePtr core = this->createBModeCore(par, algo, validInputData);
		if (core)
			retval.push_back(core);
		core = this->createCore(par, algo, validInputData);
		if (core)
			retval.push_back(core);
	}
	// only one thread
	else
	{
		ReconstructCorePtr core = this->createCore(par, algo, validInputData);
		if (core)
			retval.push_back(core);
	}

	return retval;
}

ReconstructCorePtr ReconstructionExecuter::createCore(ReconstructCore::InputParams par, ReconstructionServicePtr algo, bool validInputData)
{
	if (!validInputData)
		return ReconstructCorePtr();

	ReconstructCorePtr retval(new ReconstructCore());

//	ReconstructCore::InputParams par = this->createCoreParameters();
//	ReconstructionServicePtr algo = this->createAlgorithm();

	retval->initialize(par, algo);

	return retval;
}

ReconstructCorePtr ReconstructionExecuter::createBModeCore(ReconstructCore::InputParams par, ReconstructionServicePtr algo, bool validInputData)
{
	if (!validInputData)
		return ReconstructCorePtr();

	ReconstructCorePtr retval(new ReconstructCore());

//	ReconstructCore::InputParams par = this->createCoreParameters();
	par.mAngio = false;
	par.mTransferFunctionPreset = "US B-Mode";
//	ReconstructionServicePtr algo = this->createAlgorithm();

	retval->initialize(par, algo);

	return retval;
}



} /* namespace cx */
