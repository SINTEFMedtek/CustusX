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
typedef boost::shared_ptr<class ThreadedTimedReconstructPreprocessor> ThreadedTimedReconstructPreprocessorPtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructCore> ThreadedTimedReconstructCorePtr;


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

	/** Return the currently reconstructing thread object.
	  */
	std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction() { return mThreadedReconstruction; }

	/** Execute the reconstruction in another thread.
	  *
	  * The returned cores can be used to retrieve output,
	  * but this must be done AFTER the threads have completed.
	  * In general, dont use the retval, it is for unit testing.
	  */
	std::vector<ReconstructCorePtr> startReconstruction();

signals:
	void paramsChanged();
	void algorithmChanged();
	void inputDataSelected(QString mhdFileName);
	void reconstructAboutToStart();

private:
	ReconstructerPtr mReconstructer;
	std::set<cx::TimedAlgorithmPtr> mThreadedReconstruction;
//	cx::UsReconstructionFileReaderPtr mFileReader;
	ssc::USReconstructInputData mOriginalFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.
//	QString mCalFilesPath; ///< Path to calibration files

	void launch(cx::TimedAlgorithmPtr thread);
	void readCoreFiles(QString fileName, QString calFilesPath);
	void clearAll();
	bool validInputData() const;

private slots:
	void threadFinishedSlot();

};


/**
 * @}
 */
}

#endif /* SSCRECONSTRUCTMANAGER_H_ */
