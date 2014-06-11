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

	/** Execute the reconstruction in asynchronously.
	  * When reconstructFinished() is emitted, use getResult().
	  */
	void startReconstruction(ReconstructionServicePtr algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio);
	std::vector<cx::ImagePtr> getResult(); // return latest reconstruct result (after reconstructFinished() emitted), empty during processing.
	cx::TimedAlgorithmPtr getThread(); ///< Return the currently reconstructing thread object.
	void startNonThreadedReconstruction(ReconstructionServicePtr algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio);

signals:
	void reconstructAboutToStart(); ///< emitted before reconstruction threads are fired
	void reconstructStarted();
	void reconstructFinished();

private:
	/**
	  * Create the reconstruct preprocessor object.
	  * This is usually created internally during reconstruction,
	  * published for use in unit testing.
	  */
	ReconstructPreprocessorPtr createPreprocessor(ReconstructCore::InputParams par, USReconstructInputData fileData);
	/**
	  * Create the reconstruct core object.
	  * This is usually created internally during reconstruction,
	  * published for use in unit testing.
	  */
	std::vector<ReconstructCorePtr> createCores(ReconstructionServicePtr algo, ReconstructCore::InputParams par, bool createBModeWhenAngio); ///< create reconstruct cores matching the current parameters

	void launch(cx::TimedAlgorithmPtr thread);
	ReconstructCorePtr createCore(ReconstructCore::InputParams par, ReconstructionServicePtr algo); ///< used for threaded reconstruction
	ReconstructCorePtr createBModeCore(ReconstructCore::InputParams par, ReconstructionServicePtr algo); ///< core version for B-mode in case of angio recording.
	cx::CompositeTimedAlgorithmPtr assembleReconstructionPipeline(std::vector<ReconstructCorePtr> cores, ReconstructCore::InputParams par, USReconstructInputData fileData); ///< assembles the different steps that is needed to reconstruct
	bool canCoresRunInParallel(std::vector<ReconstructCorePtr> cores);

	std::vector<ReconstructCorePtr> mCores;
	cx::TimedAlgorithmPtr mPipeline;
};

} /* namespace cx */

#endif /* CXRECONSTRUCTIONEXECUTER_H_ */
