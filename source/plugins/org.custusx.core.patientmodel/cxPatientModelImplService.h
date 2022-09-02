/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	virtual void insertData(DataPtr data, bool overWrite = false);
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
	FileManagerServicePtr mFileManagerService;

	std::map<QString, ToolPtr> mProbeTools;

	ActiveDataPtr mActiveData;

	std::vector<QString> mUnavailableData;

	void disconnectProbes();
	void connectProbes();
};
typedef boost::shared_ptr<PatientModelImplService> PatientModelImplServicePtr;

} /* namespace cx */

#endif /* CXPATIENTMODELIMPLSERVICE_H_ */

