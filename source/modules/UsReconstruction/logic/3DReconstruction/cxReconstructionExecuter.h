#ifndef CXRECONSTRUCTIONEXECUTER_H_
#define CXRECONSTRUCTIONEXECUTER_H_

#include "boost/shared_ptr.hpp"
#include <QObject>
#include <set>

#include "cxReconstructionService.h"
#include "cxReconstructPreprocessor.h"
#include "cxReconstructParams.h"
#include "cxCompositeTimedAlgorithm.h"

namespace cx
{
typedef boost::shared_ptr<class TimedBaseAlgorithm> TimedAlgorithmPtr;
typedef boost::shared_ptr<class ReconstructionExecuter> ReconstructionExecuterPtr;

class ReconstructionExecuter : public QObject
{
	Q_OBJECT
public:
	ReconstructionExecuter() {}

	/** Execute the reconstruction in another thread.
	  *
	  * The returned cores can be used to retrieve output,
	  * but this must be done AFTER the threads have completed.
	  * In general, dont use the retval, it is for unit testing.
	  */
	virtual std::vector<ReconstructCorePtr> startReconstruction(ReconstructionServicePtr algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio);
	virtual std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction(); ///< Return the currently reconstructing thread object(s).
	/**
	  * Create the reconstruct preprocessor object.
	  * This is usually created internally during reconstruction,
	  * published for use in unit testing.
	  */
	virtual ReconstructPreprocessorPtr createPreprocessor(ReconstructCore::InputParams par, USReconstructInputData fileData);
	/**
	  * Create the reconstruct core object.
	  * This is usually created internally during reconstruction,
	  * published for use in unit testing.
	  */
	virtual std::vector<ReconstructCorePtr> createCores(ReconstructionServicePtr algo, ReconstructCore::InputParams par, bool createBModeWhenAngio); ///< create reconstruct cores matching the current parameters


signals:
	void reconstructAboutToStart(); ///< emitted before reconstruction threads are fired
	void reconstructStarted();
	void reconstructFinished();

private slots:
	void threadFinishedSlot();

private:
	void launch(cx::TimedAlgorithmPtr thread);
	ReconstructCorePtr createCore(ReconstructCore::InputParams par, ReconstructionServicePtr algo); ///< used for threaded reconstruction
	ReconstructCorePtr createBModeCore(ReconstructCore::InputParams par, ReconstructionServicePtr algo); ///< core version for B-mode in case of angio recording.
	cx::CompositeTimedAlgorithmPtr assembleReconstructionPipeline(std::vector<ReconstructCorePtr> cores, ReconstructCore::InputParams par, USReconstructInputData fileData); ///< assembles the different steps that is needed to reconstruct
	bool canCoresRunInParallel(std::vector<ReconstructCorePtr> cores);


	std::set<cx::TimedAlgorithmPtr> mThreadedReconstruction;
};

} /* namespace cx */

#endif /* CXRECONSTRUCTIONEXECUTER_H_ */
