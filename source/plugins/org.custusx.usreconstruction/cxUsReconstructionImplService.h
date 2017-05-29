/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXUSRECONSTRUCTIONIMPLSERVICE_H
#define CXUSRECONSTRUCTIONIMPLSERVICE_H

#include "org_custusx_usreconstruction_Export.h"
#include "cxUsReconstructionService.h"

#include "cxUSReconstructInputData.h"
#include "cxReconstructionMethodService.h"
#include "cxServiceTrackerListener.h"

class ctkPluginContext;

namespace cx
{


/**
 * \file
 * \addtogroup org_custusx_usreconstruction
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
class org_custusx_usreconstruction_EXPORT UsReconstructionImplService : public UsReconstructionService
{
	Q_OBJECT
	Q_INTERFACES(cx::UsReconstructionService)

public:
	UsReconstructionImplService(ctkPluginContext* pluginContext, PatientModelServicePtr patientModelService, ViewServicePtr viewService, FileManagerServicePtr filemanagerservice, XmlOptionFile settings);
	virtual ~UsReconstructionImplService();
	virtual bool isNull();

	virtual void selectData(QString filename, QString calFilesPath = ""); ///< Set input data for reconstruction
	virtual void selectData(USReconstructInputData data); ///< Set input data for reconstruction

	virtual QString getSelectedFilename() const; ///< Get the currently selected filename
	virtual USReconstructInputData getSelectedFileData(); ///< Return the currently selected input data
	virtual PropertyPtr getParam(QString uid); ///< Return one of the standard parameters

	virtual std::vector<PropertyPtr> getAlgoOptions(); ///< Return control parameters for the currently selected algorithm, adjustable like getParams()
	virtual XmlOptionFile getSettings(); ///< Return the settings xml file where parameters are stored
	virtual OutputVolumeParams getOutputVolumeParams() const; ///< Return params controlling the output data. These are data-dependent.

	virtual void setOutputVolumeParams(const OutputVolumeParams& par); ///< Control the output volume

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
	virtual ReconstructionMethodService* createAlgorithm();

	virtual ReconstructCore::InputParams createCoreParameters();

public slots:
	virtual void newDataOnDisk(QString mhdFilename);

private slots:
	void setSettings();
	void reconstructFinishedSlot();

	void patientChangedSlot();

private:
	void clearAll();

	/** Use the mOriginalFileData structure to rebuild all internal data.
	 *  Useful when settings have changed or data is loaded.
	 */
	void updateFromOriginalFileData();

	void onServiceAdded(ReconstructionMethodService* service);
	void onServiceModified(ReconstructionMethodService* service);
	void onServiceRemoved(ReconstructionMethodService* service);

	ReconstructParamsPtr mParams;
	std::vector<PropertyPtr> mAlgoOptions;
	USReconstructInputData mOriginalFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.

	OutputVolumeParams mOutputVolumeParams;
	XmlOptionFile mSettings;
	QString mShaderPath; ///< name of shader folder

	boost::shared_ptr<ServiceTrackerListener<ReconstructionMethodService> > mServiceListener;
	std::vector<ReconstructionExecuterPtr> mExecuters;

	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
	FileManagerServicePtr mFileManagerService;
};

/**
 * @}
 */
}



#endif // CXUSRECONSTRUCTIONIMPLSERVICE_H
