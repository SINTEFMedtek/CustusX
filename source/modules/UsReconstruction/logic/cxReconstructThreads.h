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
#ifndef CXRECONSTRUCTTHREADS_H_
#define CXRECONSTRUCTTHREADS_H_

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
 * \addtogroup cx_module_usreconstruction
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
class ThreadedTimedReconstructPreprocessor: public cx::ThreadedTimedAlgorithm<void>
{
Q_OBJECT
public:
	static ThreadedTimedReconstructPreprocessorPtr create(ReconstructPreprocessorPtr input, std::vector<ReconstructCorePtr> cores)
	{
		return ThreadedTimedReconstructPreprocessorPtr(new ThreadedTimedReconstructPreprocessor(input, cores));
	}

	ThreadedTimedReconstructPreprocessor(ReconstructPreprocessorPtr input, std::vector<ReconstructCorePtr> cores);
	virtual ~ThreadedTimedReconstructPreprocessor();

private slots:
	virtual void preProcessingSlot();
	virtual void postProcessingSlot();

private:
	virtual void calculate();
	ReconstructPreprocessorPtr mInput;
	std::vector<ReconstructCorePtr> mCores;
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
class ThreadedTimedReconstructCore: public cx::ThreadedTimedAlgorithm<void>
{
Q_OBJECT
public:
	static ThreadedTimedReconstructCorePtr create(ReconstructCorePtr reconstructer)
	{
		return ThreadedTimedReconstructCorePtr(new ThreadedTimedReconstructCore(reconstructer));
	}
	ThreadedTimedReconstructCore(ReconstructCorePtr reconstructer);
	virtual ~ThreadedTimedReconstructCore();

private slots:
	virtual void preProcessingSlot();
	virtual void postProcessingSlot();

private:
	virtual void calculate();
	ReconstructCorePtr mReconstructer;
};


/**
 * @}
 */
}

#endif // CXRECONSTRUCTTHREADS_H_
