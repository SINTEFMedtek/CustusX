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
	void startReconstruction(ReconstructionMethodService* algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio);
	std::vector<cx::ImagePtr> getResult(); // return latest reconstruct result (after reconstructFinished() emitted), empty during processing.
	cx::TimedAlgorithmPtr getThread(); ///< Return the currently reconstructing thread object.
	void startNonThreadedReconstruction(ReconstructionMethodService* algo, ReconstructCore::InputParams par, USReconstructInputData fileData, bool createBModeWhenAngio);

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
