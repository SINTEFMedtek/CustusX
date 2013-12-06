// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCRECONSTRUCTMANAGER_H_
#define SSCRECONSTRUCTMANAGER_H_

#include <set>
#include "cxForwardDeclarations.h"
#include "sscXmlOptionItem.h"
#include "sscReconstructCore.h"
#include "cxUSReconstructInputData.h"
#include "sscReconstructedOutputVolumeParams.h"

namespace cx
{
typedef boost::shared_ptr<class TimedBaseAlgorithm> TimedAlgorithmPtr;
}

namespace cx
{
typedef boost::shared_ptr<class CompositeTimedAlgorithm> CompositeTimedAlgorithmPtr;
typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
typedef boost::shared_ptr<class ReconstructCore> ReconstructCorePtr;
typedef boost::shared_ptr<class ReconstructParams> ReconstructParamsPtr;
typedef boost::shared_ptr<class ReconstructPreprocessor> ReconstructPreprocessorPtr;
typedef boost::shared_ptr<class ReconstructAlgorithm> ReconstructAlgorithmPtr;

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

	void selectData(QString filename, QString calFilesPath = ""); ///< Set input data for reconstruction
	void selectData(USReconstructInputData data); ///< Set input data for reconstruction

	QString getSelectedFilename() const; ///< Get the currently selected filename
	USReconstructInputData getSelectedFileData(); ///< Return the currently selected input data
	ReconstructParamsPtr getParams(); ///< Return control parameters that can be adjusted by the GUI or similar prior to reconstruction
	std::vector<DataAdapterPtr> getAlgoOptions(); ///< Return control parameters for the currently selected algorithm, adjustable like getParams()
	XmlOptionFile getSettings(); ///< Return the settings xml file where parameters are stored
	OutputVolumeParams getOutputVolumeParams() const; ///< Return params controlling the output data. These are data-dependent.

	void setOutputVolumeParams(const OutputVolumeParams& par); ///< Control the output volume
	void setOutputRelativePath(QString path); ///< Set location of output relative to base
	void setOutputBasePath(QString path); ///< Set base location of output

	/** Execute the reconstruction in another thread.
	  *
	  * The returned cores can be used to retrieve output,
	  * but this must be done AFTER the threads have completed.
	  * In general, dont use the retval, it is for unit testing.
	  */
	std::vector<ReconstructCorePtr> startReconstruction();
	std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction(); ///< Return the currently reconstructing thread object(s).
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

private slots:
	void threadFinishedSlot();

private:
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
	cx::CompositeTimedAlgorithmPtr assembleReconstructionPipeline(std::vector<ReconstructCorePtr> cores); ///< assembles the different steps that is needed to reconstruct
	bool canCoresRunInParallel(std::vector<ReconstructCorePtr> cores);

	ReconstructParamsPtr mParams;
	std::vector<DataAdapterPtr> mAlgoOptions;
	std::set<cx::TimedAlgorithmPtr> mThreadedReconstruction;
	USReconstructInputData mOriginalFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.

	OutputVolumeParams mOutputVolumeParams;
	XmlOptionFile mSettings;
	QString mOutputRelativePath;///< Relative path to the output image
	QString mOutputBasePath;///< Global path where the relative path starts, for the output image
	QString mShaderPath; ///< name of shader folder

};

/**
 * @}
 */
}

#endif /* SSCRECONSTRUCTMANAGER_H_ */
