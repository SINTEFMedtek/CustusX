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
#include "cxThreadedTimedAlgorithm.h"
#include "sscReconstructPreprocessor.h"
#include "sscReconstructParams.h"
#include "sscReconstructCore.h"
#include "sscReconstructPreprocessor.h"

namespace ssc
{

typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
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
 * \brief Manager for the us reconstruction process.
 *
 * Create a 3D volume based on a set of 2D images with 3D position.
 *
 * \verbatim
 * Used coordinate systems:
 * u  = raw input Ultrasound frames (in x, y. Origin lower left.)
 * t  = Tool space for probe as defined in ssc:Tool (z in ray direction, y to the left)
 * s  = probe localizer Sensor.
 * pr = Patient Reference localizer sensor.
 * d  = Output Data space
 * \endverbatim
 *
 * \author Ole Vegard Solberg
 * \author Christian Askeland
 * \date May 4, 2010
 */
class ReconstructManager: public QObject
{
Q_OBJECT
	friend class ThreadedReconstructer;

public:
	ReconstructManager(XmlOptionFile settings, QString shaderPath);
	virtual ~ReconstructManager();

	/**
	  * Set input data for reconstruction
	  */
	void selectData(QString filename, QString calFilesPath = "");
	/**
	  * Set input data for reconstruction
	  */
	void selectData(ssc::USReconstructInputData data);
	/**
	  * Get the currently selected filename
	  */
	QString getSelectedFilename() const;
	/**
	  * Return the currently selected input data
	  */
	ssc::USReconstructInputData getSelectedFileData() { return mOriginalFileData; }

	/**
	  * Return control parameters that can be adjusted by the GUI or similar prior to reconstruction
	  */
	ReconstructParamsPtr getParams();
	/**
	  * Return control parameters for the currently selected algorithm, adjustable like getParams()
	  */
	std::vector<DataAdapterPtr> getAlgoOptions();
	/**
	  * Return the settings xml file where parameters are stored
	  */
	XmlOptionFile getSettings() { return mSettings; }
	/**
	  * Return params controlling the output data. These are data-dependent.
	  */
	OutputVolumeParams getOutputVolumeParams() const;
	/**
	  * Control the output volume
	  */
	void setOutputVolumeParams(const OutputVolumeParams& par);
	/**
	  * Set location of output relative to base
	  */
	void setOutputRelativePath(QString path);
	/**
	  * Set base location of output
	  */
	void setOutputBasePath(QString path);

	/** Execute the reconstruction in another thread.
	  *
	  * The returned cores can be used to retrieve output,
	  * but this must be done AFTER the threads have completed.
	  * In general, dont use the retval, it is for unit testing.
	  */
	std::vector<ReconstructCorePtr> startReconstruction();
	/** Return the currently reconstructing thread object(s).
	  */
	std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction() { return mThreadedReconstruction; }

	/**
	  * Create the reconstruct preprocessor object.
	  * This is usually created internally during reconstruction,
	  * published for use in unit testing.
	  */
	ReconstructPreprocessorPtr createPreprocessor();
	/**
	  * Create the reconstruct core object.
	  * This is usually created internally during reconstruction,
	  * published for use in unit testing.
	  */
	std::vector<ReconstructCorePtr> createCores(); ///< create reconstruct cores matching the current parameters
	/**
	  * Create the reconstruct algorithm object.
	  * This is usually created internally during reconstruction,
	  * published for use in unit testing.
	  */
	ReconstructAlgorithmPtr createAlgorithm();

private slots:
	void setSettings();
	void transferFunctionChangedSlot();

signals:
	void paramsChanged();
	void algorithmChanged();
	void inputDataSelected(QString mhdFileName);
	void reconstructAboutToStart();

private:
	ReconstructParamsPtr mParams;
	std::vector<DataAdapterPtr> mAlgoOptions;
	std::set<cx::TimedAlgorithmPtr> mThreadedReconstruction;
	ssc::USReconstructInputData mOriginalFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.

	OutputVolumeParams mOutputVolumeParams;
	XmlOptionFile mSettings;
	QString mOutputRelativePath;///< Relative path to the output image
	QString mOutputBasePath;///< Global path where the relative path starts, for the output image
	QString mShaderPath; ///< name of shader folder

	void launch(cx::TimedAlgorithmPtr thread);
	void clearAll();
	ReconstructCorePtr createCore(); ///< used for threaded reconstruction
	ReconstructCorePtr createBModeCore(); ///< core version for B-mode in case of angio recording.

	void initAlgorithm();
	/** Use the mOriginalFileData structure to rebuild all internal data.
	 *  Useful when settings have changed or data is loaded.
	 */
	void updateFromOriginalFileData();
	ReconstructCore::InputParams createCoreParameters();

	bool validInputData() const;///< checks if internal states is valid (that it actually has frames to reconstruct)

private slots:
	void threadFinishedSlot();

};


/**
 * @}
 */
}

#endif /* SSCRECONSTRUCTMANAGER_H_ */
