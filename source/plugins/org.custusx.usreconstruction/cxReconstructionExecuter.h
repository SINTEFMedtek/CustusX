/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRECONSTRUCTIONEXECUTER_H_
#define CXRECONSTRUCTIONEXECUTER_H_

#include "org_custusx_usreconstruction_Export.h"

#include "boost/shared_ptr.hpp"
#include <QObject>
#include <set>

#include "cxReconstructionMethodService.h"
#include "cxReconstructPreprocessor.h"
#include "cxCompositeTimedAlgorithm.h"

namespace cx
{
typedef boost::shared_ptr<class TimedBaseAlgorithm> TimedAlgorithmPtr;
typedef boost::shared_ptr<class ReconstructionExecuter> ReconstructionExecuterPtr;

/**
 * \ingroup org_custusx_usreconstruction
 */
class org_custusx_usreconstruction_EXPORT ReconstructionExecuter : public QObject
{
	Q_OBJECT
public:
	ReconstructionExecuter(PatientModelServicePtr patientModelService, ViewServicePtr viewService) :
	mPatientModelService(patientModelService),
	mViewService(viewService)
	{}

	/** Execute the reconstruction in asynchronously.
	  * When reconstructFinished() is emitted, use getResult().
	  */
	bool startReconstruction(ReconstructionMethodService* algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio);
	std::vector<cx::ImagePtr> getResult(); // return latest reconstruct result (after reconstructFinished() emitted), empty during processing.
	cx::TimedAlgorithmPtr getThread(); ///< Return the currently reconstructing thread object.
	bool startNonThreadedReconstruction(ReconstructionMethodService* algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio);

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
	std::vector<ReconstructCorePtr> createCores(ReconstructionMethodService* algo, ReconstructCore::InputParams par, bool createBModeWhenAngio); ///< create reconstruct cores matching the current parameters

	void launch(cx::TimedAlgorithmPtr thread);
	ReconstructCorePtr createCore(ReconstructCore::InputParams par, ReconstructionMethodService* algo); ///< used for threaded reconstruction
	ReconstructCorePtr createBModeCore(ReconstructCore::InputParams par, ReconstructionMethodService* algo); ///< core version for B-mode in case of angio recording.
	cx::CompositeTimedAlgorithmPtr assembleReconstructionPipeline(std::vector<ReconstructCorePtr> cores, ReconstructCore::InputParams par, USReconstructInputData fileData); ///< assembles the different steps that is needed to reconstruct
	bool canCoresRunInParallel(std::vector<ReconstructCorePtr> cores);

	std::vector<ReconstructCorePtr> mCores;
	cx::TimedAlgorithmPtr mPipeline;
	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
};

} /* namespace cx */

#endif /* CXRECONSTRUCTIONEXECUTER_H_ */
