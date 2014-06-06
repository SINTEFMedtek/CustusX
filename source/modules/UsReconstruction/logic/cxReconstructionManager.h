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

#ifndef CXRECONSTRUCTIONMANAGER_H_
#define CXRECONSTRUCTIONMANAGER_H_

#include <set>
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxReconstructCore.h"
#include "cxUSReconstructInputData.h"
#include "cxReconstructedOutputVolumeParams.h"
#include "cxReconstructionService.h"
#include "cxServiceTrackerListener.h"

namespace cx
{
typedef boost::shared_ptr<class TimedBaseAlgorithm> TimedAlgorithmPtr;
typedef boost::shared_ptr<class CompositeTimedAlgorithm> CompositeTimedAlgorithmPtr;
typedef boost::shared_ptr<class ReconstructionManager> ReconstructionManagerPtr;
typedef boost::shared_ptr<class ReconstructCore> ReconstructCorePtr;
typedef boost::shared_ptr<class ReconstructParams> ReconstructParamsPtr;
typedef boost::shared_ptr<class ReconstructPreprocessor> ReconstructPreprocessorPtr;
typedef boost::shared_ptr<class ReconstructionService> ReconstructionServicePtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructer> ThreadedTimedReconstructerPtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructPreprocessor> ThreadedTimedReconstructPreprocessorPtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructCore> ThreadedTimedReconstructCorePtr;

typedef boost::shared_ptr<class ReconstructionExecuter> ReconstructionExecuterPtr;


/**
 * \file
 * \addtogroup cx_module_usreconstruction
 * @{
 */

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
 * \author Janne Beate Bakeng
 * \date May 4, 2010
 */
class ReconstructionManager : public QObject
{
Q_OBJECT

public:
	ReconstructionManager(XmlOptionFile settings, QString shaderPath);
	virtual ~ReconstructionManager();

	virtual void init();

	virtual void selectData(QString filename, QString calFilesPath = ""); ///< Set input data for reconstruction
	virtual void selectData(USReconstructInputData data); ///< Set input data for reconstruction

	virtual QString getSelectedFilename() const; ///< Get the currently selected filename
	virtual USReconstructInputData getSelectedFileData(); ///< Return the currently selected input data
	virtual ReconstructParamsPtr getParams(); ///< Return control parameters that can be adjusted by the GUI or similar prior to reconstruction
	virtual std::vector<DataAdapterPtr> getAlgoOptions(); ///< Return control parameters for the currently selected algorithm, adjustable like getParams()
	virtual XmlOptionFile getSettings(); ///< Return the settings xml file where parameters are stored
	virtual OutputVolumeParams getOutputVolumeParams() const; ///< Return params controlling the output data. These are data-dependent.

	virtual void setOutputVolumeParams(const OutputVolumeParams& par); ///< Control the output volume
	virtual void setOutputRelativePath(QString path); ///< Set location of output relative to base
	virtual void setOutputBasePath(QString path); ///< Set base location of output

	/** Execute the reconstruction in another thread.
	  *
	  * The returned cores can be used to retrieve output,
	  * but this must be done AFTER the threads have completed.
	  * In general, dont use the retval, it is for unit testing.
	  */
	virtual void startReconstruction();
	virtual std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction(); ///< Return the currently reconstructing thread object(s).
	/**
	  * Create the reconstruct algorithm object.
	  * This is usually created internally during reconstruction,
	  * published for use in unit testing.
	  */
	virtual ReconstructionServicePtr createAlgorithm();

	virtual ReconstructCore::InputParams createCoreParameters();

signals:
	void paramsChanged();
	void algorithmChanged();
	void inputDataSelected(QString mhdFileName);
	void reconstructAboutToStart();
	void reconstructStarted();
	void reconstructFinished();

	void newInputDataAvailable(QString mhdFileName);

private slots:
	void setSettings();
	void transferFunctionChangedSlot();
	void reconstructFinishedSlot();

private:
	void clearAll();

	void initAlgorithm();
	/** Use the mOriginalFileData structure to rebuild all internal data.
	 *  Useful when settings have changed or data is loaded.
	 */
	void updateFromOriginalFileData();

    void onServiceAdded(ReconstructionService* service);
    void onServiceModified(ReconstructionService* service);
    void onServiceRemoved(ReconstructionService* service);

	ReconstructParamsPtr mParams;
	std::vector<DataAdapterPtr> mAlgoOptions;
	USReconstructInputData mOriginalFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.

	OutputVolumeParams mOutputVolumeParams;
	XmlOptionFile mSettings;
	QString mOutputRelativePath;///< Relative path to the output image
	QString mOutputBasePath;///< Global path where the relative path starts, for the output image
	QString mShaderPath; ///< name of shader folder

	boost::shared_ptr<ServiceTrackerListener<ReconstructionService> > mServiceListener;
	std::vector<ReconstructionExecuterPtr> mExecuters;
};

/**
 * @}
 */
}

#endif /* CXRECONSTRUCTIONMANAGER_H_ */
