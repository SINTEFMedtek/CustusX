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
#ifndef SSCRECONSTRUCTTHREADS_H
#define SSCRECONSTRUCTTHREADS_H

#include <QObject>
#include <QThread>
#include <math.h>
#include "sscReconstructAlgorithm.h"
#include "sscBoundingBox3D.h"
#include "sscReconstructedOutputVolumeParams.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"
#include "sscXmlOptionItem.h"
#include "sscProbeSector.h"
#include "cxThreadedTimedAlgorithm.h"
#include "sscReconstructPreprocessor.h"

namespace ssc
{

//typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
//typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
//typedef boost::shared_ptr<class ReconstructCore> ReconstructCorePtr;
//typedef boost::shared_ptr<class ReconstructParams> ReconstructParamsPtr;

/**
 * \file
 * \addtogroup cxPluginUsReconstruction
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

//	void start();

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

#endif // SSCRECONSTRUCTTHREADS_H
