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
//#include "probeXmlConfigParser.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"
#include "sscXmlOptionItem.h"
#include "sscProbeSector.h"
//#include "sscStringWidgets.h"
#include "cxUsReconstructionFileReader.h"

namespace ssc
{

typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
typedef boost::shared_ptr<class ReconstructParams> ReconstructParamsPtr;

/**
 * \file
 * \addtogroup cxPluginUsReconstruction
 * @{
 */

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
	void reconstruct(); // assumes readFiles has already been called
	QString getSelectedData() const;

//	StringDataAdapterXmlPtr mOrientationAdapter;
//	StringDataAdapterXmlPtr mPresetTFAdapter;
//	StringDataAdapterXmlPtr mAlgorithmAdapter;
//	StringDataAdapterXmlPtr mMaskReduce;//Reduce mask size in % in each direction
//	BoolDataAdapterXmlPtr mAlignTimestamps; ///align track and frame timestamps to each other automatically
//	DoubleDataAdapterXmlPtr mTimeCalibration; ///set a offset in the frame timestamps
//	BoolDataAdapterXmlPtr mAngioAdapter; ///US angio data is used as input
	ReconstructParamsPtr getParams();

	ReconstructAlgorithmPtr getAlgorithm();///< The used reconstruction algorithm
	std::vector<DataAdapterPtr> getAlgoOptions();
	ReconstructerPtr getReconstructer() { return mReconstructer; }

	ImagePtr getOutput();
	OutputVolumeParams getOutputVolumeParams() const;
	void setOutputVolumeParams(const OutputVolumeParams& par);
	void setOutputRelativePath(QString path);
	void setOutputBasePath(QString path);

//public slots:
//	void setSettings();

signals:
	void paramsChanged();
	void algorithmChanged();
	void inputDataSelected(QString mhdFileName);
	void reconstructFinished();

private:
	ReconstructerPtr mReconstructer;

	cx::UsReconstructionFileReaderPtr mFileReader;
	ssc::USReconstructInputData mOriginalFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.
	QString mCalFilesPath; ///< Path to calibration files
//	QString mFilename; ///< filename used for current data read

	void readCoreFiles(QString fileName, QString calFilesPath);
	void clearAll();
	void updateFromOriginalFileData();
	bool validInputData() const;
};

/**Execution of a reconstruction in another thread.
 * The class replaces the ReconstructManager::reconstruct() method.
 *
 */
class ThreadedReconstructer: public QThread
{
Q_OBJECT

public:
	ThreadedReconstructer(ReconstructManagerPtr reconstructer);
	virtual void run();
private slots:
	void postReconstructionSlot();
private:
	ReconstructManagerPtr mReconstructer;
};
typedef boost::shared_ptr<class ThreadedReconstructer> ThreadedReconstructerPtr;


/**
 * @}
 */
}

#endif /* SSCRECONSTRUCTMANAGER_H_ */
