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
#ifndef CXRECONSTRUCTTHREADS_H_
#define CXRECONSTRUCTTHREADS_H_

#include "org_custusx_usreconstruction_Export.h"

#include <QObject>
#include <QThread>
#include <math.h>
#include "cxForwardDeclarations.h"
#include "cxThreadedTimedAlgorithm.h"

namespace cx
{
typedef boost::shared_ptr<class ReconstructPreprocessor> ReconstructPreprocessorPtr;
typedef boost::shared_ptr<class ReconstructCore> ReconstructCorePtr;

/**
 * \file
 * \addtogroup org_custusx_usreconstruction
 * @{
 */


typedef boost::shared_ptr<class ThreadedTimedReconstructer> ThreadedTimedReconstructerPtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructPreprocessor> ThreadedTimedReconstructPreprocessorPtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructCore> ThreadedTimedReconstructCorePtr;

/**
 * \brief Threading adapter for the reconstruction algorithm.
 *
 * Must be run before ThreadedTimedReconstructCore.
 *
 * Executes ReconstructCore functions:
 *  - threadedPreReconstruct() [main thread]
 *  - threadablePreReconstruct() [work thread]
 *
 * \date Jan 27, 2012
 * \author Christian Askeland, SINTEF
 */
class org_custusx_usreconstruction_EXPORT ThreadedTimedReconstructPreprocessor: public cx::ThreadedTimedAlgorithm<void>
{
Q_OBJECT
public:
	static ThreadedTimedReconstructPreprocessorPtr create(PatientModelServicePtr patientModelService, ReconstructPreprocessorPtr input, std::vector<ReconstructCorePtr> cores)
	{
		return ThreadedTimedReconstructPreprocessorPtr(new ThreadedTimedReconstructPreprocessor(patientModelService, input, cores));
	}

	ThreadedTimedReconstructPreprocessor(PatientModelServicePtr patientModelService, ReconstructPreprocessorPtr input, std::vector<ReconstructCorePtr> cores);
	virtual ~ThreadedTimedReconstructPreprocessor();

private slots:
	virtual void preProcessingSlot();
	virtual void postProcessingSlot();

private:
	virtual void calculate();
	ReconstructPreprocessorPtr mInput;
	std::vector<ReconstructCorePtr> mCores;
	PatientModelServicePtr mPatientModelService;
};

/**
 * \brief Threading adapter for the reconstruction algorithm.
 *
 * Must be run after ThreadedTimedReconstructerStep2.
 *
 * Executes ReconstructCore functions:
 *  - threadedReconstruct() [work thread]
 *  - threadedPostReconstruct() [main thread]
 *
 * \date Jan 27, 2012
 * \author Christian Askeland, SINTEF
 */
//template class org_custusx_usreconstruction_EXPORT ThreadedTimedAlgorithm<void>;
class org_custusx_usreconstruction_EXPORT ThreadedTimedReconstructCore: public cx::ThreadedTimedAlgorithm<void>
{
Q_OBJECT
public:
	static ThreadedTimedReconstructCorePtr create(PatientModelServicePtr patientModelService, ViewServicePtr viewService, ReconstructCorePtr reconstructer)
	{
		return ThreadedTimedReconstructCorePtr(new ThreadedTimedReconstructCore(patientModelService, viewService, reconstructer));
	}
	ThreadedTimedReconstructCore(PatientModelServicePtr patientModelService, ViewServicePtr viewService, ReconstructCorePtr reconstructer);
	virtual ~ThreadedTimedReconstructCore();

private slots:
	virtual void preProcessingSlot();
	virtual void postProcessingSlot();

private:
	virtual void calculate();
	ReconstructCorePtr mReconstructer;
	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
};


/**
 * @}
 */
}

#endif // CXRECONSTRUCTTHREADS_H_
