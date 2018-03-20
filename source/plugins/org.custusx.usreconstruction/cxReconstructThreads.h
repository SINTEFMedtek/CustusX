/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
