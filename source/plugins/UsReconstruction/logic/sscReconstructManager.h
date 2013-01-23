/*
 * sscReconstructManager.h
 *
 *  \date Oct 4, 2011
 *      \author christiana
 */

#ifndef SSCRECONSTRUCTMANAGER_H_
#define SSCRECONSTRUCTMANAGER_H_

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
#include "cxUsReconstructionFileReader.h"
#include "cxThreadedTimedAlgorithm.h"
#include "sscReconstructPreprocessor.h"

namespace ssc
{

typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
typedef boost::shared_ptr<class ReconstructCore> ReconstructCorePtr;
typedef boost::shared_ptr<class ReconstructParams> ReconstructParamsPtr;

/**
 * \file
 * \addtogroup cxPluginUsReconstruction
 * @{
 */


typedef boost::shared_ptr<class ThreadedTimedReconstructer> ThreadedTimedReconstructerPtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructerStep1> ThreadedTimedReconstructerStep1Ptr;
typedef boost::shared_ptr<class ThreadedTimedReconstructerStep2> ThreadedTimedReconstructerStep2Ptr;


/**
 * \verbatim
 * Used coordinate systems:
 * u  = raw input Ultrasound frames (in x, y. Origin lower left.)
 * t  = Tool space for probe as defined in ssc:Tool (z in ray direction, y to the left)
 * s  = probe localizer Sensor.
 * pr = Patient Reference localizer sensor.
 * d  = Output Data space
 * \endverbatim
 *
 */
class ReconstructManager: public QObject
{
Q_OBJECT
	friend class ThreadedReconstructer;

public:
	ReconstructManager(XmlOptionFile settings, QString shaderPath);
	virtual ~ReconstructManager();

	void selectData(QString filename, QString calFilesPath = "");
	void selectData(ssc::USReconstructInputData data);
	QString getSelectedData() const;

	ReconstructParamsPtr getParams();

	std::vector<DataAdapterPtr> getAlgoOptions();
	ReconstructerPtr getReconstructer() { return mReconstructer; }
	ssc::USReconstructInputData getBaseInputData() { return mOriginalFileData; }

	OutputVolumeParams getOutputVolumeParams() const;
	void setOutputVolumeParams(const OutputVolumeParams& par);
	void setOutputRelativePath(QString path);
	void setOutputBasePath(QString path);

	std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction() { return mThreadedReconstruction; }

	void startReconstruction();

signals:
	void paramsChanged();
	void algorithmChanged();
	void inputDataSelected(QString mhdFileName);
	void reconstructAboutToStart();

private:
	ReconstructerPtr mReconstructer;
	std::set<cx::TimedAlgorithmPtr> mThreadedReconstruction;
	cx::UsReconstructionFileReaderPtr mFileReader;
	ssc::USReconstructInputData mOriginalFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.
	QString mCalFilesPath; ///< Path to calibration files

	void launch(cx::TimedAlgorithmPtr thread);
	void readCoreFiles(QString fileName, QString calFilesPath);
	void clearAll();
	bool validInputData() const;

private slots:
	void threadFinishedSlot();

};

///**
// * \brief Threading adapter for the reconstruction algorithm.
// *
// * Executes ReconstructCore functions:
// *  - threadedPreReconstruct() [main thread]
// *  - threadablePreReconstruct() [work thread]
// *  - threadedReconstruct() [work thread]
// *  - threadedPostReconstruct() [main thread]
// *
// * \date Jan 27, 2012
// * \author Christian Askeland, SINTEF
// */
//class ThreadedTimedReconstructer: public cx::ThreadedTimedAlgorithm<void>
//{
//Q_OBJECT
//public:
//	static ThreadedTimedReconstructerPtr create(ReconstructCorePtr reconstructer)
//	{
//		return ThreadedTimedReconstructerPtr(new ThreadedTimedReconstructer(reconstructer));
//	}
//	ThreadedTimedReconstructer(ReconstructCorePtr reconstructer);
//	virtual ~ThreadedTimedReconstructer();

//private slots:
//	virtual void preProcessingSlot();
//	virtual void postProcessingSlot();

//private:
//	virtual void calculate();
//	ReconstructCorePtr mReconstructer;
//};

/**
 * \brief Threading adapter for the reconstruction algorithm.
 *
 * Must be run before ThreadedTimedReconstructerStep1.
 *
 * Executes ReconstructCore functions:
 *  - threadedPreReconstruct() [main thread]
 *  - threadablePreReconstruct() [work thread]
 *
 * \date Jan 27, 2012
 * \author Christian Askeland, SINTEF
 */
class ThreadedTimedReconstructerStep1: public cx::ThreadedTimedAlgorithm<void>
{
Q_OBJECT
public:
	static ThreadedTimedReconstructerStep1Ptr create(ReconstructPreprocessorPtr input, std::vector<ReconstructCorePtr> cores)
	{
		return ThreadedTimedReconstructerStep1Ptr(new ThreadedTimedReconstructerStep1(input, cores));
	}

	ThreadedTimedReconstructerStep1(ReconstructPreprocessorPtr input, std::vector<ReconstructCorePtr> cores);
	virtual ~ThreadedTimedReconstructerStep1();

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
class ThreadedTimedReconstructerStep2: public cx::ThreadedTimedAlgorithm<void>
{
Q_OBJECT
public:
	static ThreadedTimedReconstructerStep2Ptr create(ReconstructCorePtr reconstructer)
	{
		return ThreadedTimedReconstructerStep2Ptr(new ThreadedTimedReconstructerStep2(reconstructer));
	}
	ThreadedTimedReconstructerStep2(ReconstructCorePtr reconstructer);
	virtual ~ThreadedTimedReconstructerStep2();

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

#endif /* SSCRECONSTRUCTMANAGER_H_ */
