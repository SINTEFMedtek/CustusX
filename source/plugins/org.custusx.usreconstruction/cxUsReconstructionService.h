/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXUSRECONSTRUCTIONSERVICE_H
#define CXUSRECONSTRUCTIONSERVICE_H

#include "org_custusx_usreconstruction_Export.h"

#include <boost/shared_ptr.hpp>
#include <set>
#include <vector>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxReconstructCore.h"

#define UsReconstructionService_iid "cx::UsReconstructionService"

namespace cx
{
struct USReconstructInputData;
class XmlOptionFile;
class OutputVolumeParams;
class ReconstructionMethodService;

typedef boost::shared_ptr<class TimedBaseAlgorithm> TimedAlgorithmPtr;
typedef boost::shared_ptr<class CompositeTimedAlgorithm> CompositeTimedAlgorithmPtr;
typedef boost::shared_ptr<class ReconstructionManager> ReconstructionManagerPtr;
typedef boost::shared_ptr<class ReconstructCore> ReconstructCorePtr;
typedef boost::shared_ptr<class ReconstructParams> ReconstructParamsPtr;
typedef boost::shared_ptr<class ReconstructPreprocessor> ReconstructPreprocessorPtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructer> ThreadedTimedReconstructerPtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructPreprocessor> ThreadedTimedReconstructPreprocessorPtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructCore> ThreadedTimedReconstructCorePtr;

typedef boost::shared_ptr<class ReconstructionExecuter> ReconstructionExecuterPtr;

typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;

/**
 * \ingroup org_custusx_usreconstruction
 */
class org_custusx_usreconstruction_EXPORT UsReconstructionService : public QObject
{
	Q_OBJECT
public:

	virtual void selectData(QString filename, QString calFilesPath = "") = 0; ///< Set input data for reconstruction
	virtual void selectData(USReconstructInputData data) = 0; ///< Set input data for reconstruction

	virtual QString getSelectedFilename() const = 0; ///< Get the currently selected filename
	virtual USReconstructInputData getSelectedFileData() = 0; ///< Return the currently selected input data
//	virtual ReconstructParamsPtr getParams() = 0; ///< Return control parameters that can be adjusted by the GUI or similar prior to reconstruction
	virtual PropertyPtr getParam(QString uid) = 0; ///< Return one of the standard parameters
	virtual std::vector<PropertyPtr> getAlgoOptions() = 0; ///< Return control parameters for the currently selected algorithm, adjustable like getParams()
	virtual XmlOptionFile getSettings() = 0; ///< Return the settings xml file where parameters are stored
	virtual OutputVolumeParams getOutputVolumeParams() const = 0; ///< Return params controlling the output data. These are data-dependent.

	virtual void setOutputVolumeParams(const OutputVolumeParams& par) = 0; ///< Control the output volume
//	virtual void setOutputRelativePath(QString path) = 0; ///< Set location of output relative to base
//	virtual void setOutputBasePath(QString path) = 0; ///< Set base location of output

	/** Execute the reconstruction in another thread.
	  *
	  * The returned cores can be used to retrieve output,
	  * but this must be done AFTER the threads have completed.
	  * In general, dont use the retval, it is for unit testing.
	  */
	virtual void startReconstruction() = 0;
	virtual std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction() = 0; ///< Return the currently reconstructing thread object(s).
	/**
	  * Create the reconstruct algorithm object.
	  * This is usually created internally during reconstruction,
	  * published for use in unit testing.
	  */
	virtual ReconstructionMethodService* createAlgorithm() = 0;

	virtual ReconstructCore::InputParams createCoreParameters() = 0;

	virtual bool isNull() = 0;
	static UsReconstructionServicePtr getNullObject();

public slots:
	virtual void newDataOnDisk(QString mhdFilename) = 0;

signals:
	void paramsChanged();
	void algorithmChanged();
	void inputDataSelected(QString mhdFileName);
	void reconstructAboutToStart();
	void reconstructStarted();
	void reconstructFinished();

	void newInputDataAvailable(QString mhdFileName);
	void newInputDataPath(QString path);
};

} //cx
Q_DECLARE_INTERFACE(cx::UsReconstructionService, UsReconstructionService_iid)

#endif // CXUSRECONSTRUCTIONSERVICE_H
