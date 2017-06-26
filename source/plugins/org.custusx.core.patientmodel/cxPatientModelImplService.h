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

#ifndef CXPATIENTMODELIMPLSERVICE_H_
#define CXPATIENTMODELIMPLSERVICE_H_

#include "org_custusx_core_patientmodel_Export.h"
class ctkPluginContext;

#include "cxPatientModelService.h"

namespace cx
{
typedef boost::shared_ptr<class DataManagerImpl> DataManagerImplPtr;
typedef boost::shared_ptr<class PatientData> PatientDataPtr;
typedef boost::shared_ptr<class DataManager> DataServicePtr;
typedef boost::shared_ptr<class DataFactory> DataFactoryPtr;

/**
 * Implementation of PatientModelService.
 *
 * \ingroup org_custusx_core_patientmodel
 *
 * \date 2014-05-15
 * \author Christian Askeland
 */
class org_custusx_core_patientmodel_EXPORT PatientModelImplService : public PatientModelService
{
	Q_INTERFACES(cx::PatientModelService)
public:
	PatientModelImplService(ctkPluginContext* context);
	virtual ~PatientModelImplService();

	virtual void insertData(DataPtr data);
	virtual DataPtr createData(QString type, QString uid, QString name);
	virtual std::map<QString, DataPtr> getDatas(DataFilter filter) const;
	virtual DataPtr getData(const QString& uid) const;
	virtual std::map<QString, DataPtr> getChildren(QString parent_uid, QString of_type="") const;

	virtual LandmarksPtr getPatientLandmarks() const;
	virtual std::map<QString, LandmarkProperty> getLandmarkProperties() const;
	virtual void setLandmarkName(QString uid, QString name);
	virtual void setLandmarkActive(QString uid, bool active);

	virtual Transform3D get_rMpr() const; ///< get the patient registration transform
	virtual RegistrationHistoryPtr get_rMpr_History() const;

	virtual ActiveDataPtr getActiveData() const;

	virtual CLINICAL_VIEW getClinicalApplication() const;
	virtual void setClinicalApplication(CLINICAL_VIEW application);

	virtual std::map<QString, VideoSourcePtr> getStreams() const;

	virtual QString getActivePatientFolder() const;
	virtual bool isPatientValid() const;
	virtual DataPtr importData(QString fileName, QString &infoText);
	virtual void exportPatient(PATIENT_COORDINATE_SYSTEM externalSpace);
	virtual void removeData(QString uid);
	virtual PresetTransferFunctions3DPtr getPresetTransferFunctions3D() const;

	virtual void setCenter(const Vector3D& center);
	virtual Vector3D getCenter() const;
    void setOperatingTable(const OperatingTable &ot);
    OperatingTable getOperatingTable() const;

	virtual QString addLandmark();
	virtual void deleteLandmarks();

	virtual void autoSave();
	virtual bool isNull();

	virtual void makeAvailable(const QString& uid, bool available);

private slots:
	void probesChanged();
	void videoSourceAdded(VideoSourcePtr source);
private:
	ctkPluginContext *mContext;

	void createInterconnectedDataAndSpace();
	void shutdownInterconnectedDataAndSpace();

	virtual DataServicePtr dataService() const;
	PatientDataPtr patientData() const;

	ToolPtr getProbeTool(QString videoSourceUid);
	void reEmitActiveTrackedStream(TrackedStreamPtr trackedStream);

	DataManagerImplPtr mDataService;
	PatientDataPtr mPatientData;
	DataFactoryPtr mDataFactory;

	TrackingServicePtr mTrackingService;
	VideoServicePtr mVideoService;

	std::map<QString, ToolPtr> mProbeTools;

	ActiveDataPtr mActiveData;

	std::vector<QString> mUnavailableData;

	void disconnectProbes();
	void connectProbes();
};
typedef boost::shared_ptr<PatientModelImplService> PatientModelImplServicePtr;

} /* namespace cx */

#endif /* CXPATIENTMODELIMPLSERVICE_H_ */

