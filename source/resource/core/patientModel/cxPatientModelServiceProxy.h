/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPATIENTMODELSERVICEPROXY_H
#define CXPATIENTMODELSERVICEPROXY_H

#include "cxResourceExport.h"

#include "cxPatientModelService.h"
#include <boost/shared_ptr.hpp>
#include "cxServiceTrackerListener.h"
class ctkPluginContext;

namespace cx
{

/** \brief Always provides a PatientModelService
 *
 * Use the Proxy design pattern.
 * Uses ServiceTrackerListener to either provide an
 * implementation of PatientModelService or
 * the null object (PatientModelServiceNull)
 *
 *  \ingroup cx_resource_core_data
 *  \date 2014-09-10
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT PatientModelServiceProxy : public PatientModelService
{
public:
	static PatientModelServicePtr create(ctkPluginContext *pluginContext);
	PatientModelServiceProxy(ctkPluginContext *context);
	virtual ~PatientModelServiceProxy();

	virtual void insertData(DataPtr data, bool overWrite = false);
	virtual DataPtr createData(QString type, QString uid, QString name="");
	virtual std::map<QString, DataPtr> getDatas(DataFilter filter) const;
	virtual std::map<QString, DataPtr> getChildren(QString parent_uid, QString of_type="") const;
	virtual DataPtr getData(const QString& uid) const;

	virtual LandmarksPtr getPatientLandmarks() const;
	virtual std::map<QString, LandmarkProperty> getLandmarkProperties() const;
	virtual void setLandmarkName(QString uid, QString name);
	virtual void setLandmarkActive(QString uid, bool active);

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

    virtual void setOperatingTable(const OperatingTable& ot);
    virtual OperatingTable getOperatingTable() const;

	virtual QString addLandmark();
	virtual void deleteLandmarks();

	virtual void autoSave();
	virtual bool isNull();

	virtual void makeAvailable(const QString& uid, bool available);

private:
	void initServiceListener();
	void onServiceAdded(PatientModelService* service);
	void onServiceRemoved(PatientModelService *service);

	ctkPluginContext *mPluginContext;
	PatientModelServicePtr mPatientModelService;
	boost::shared_ptr<ServiceTrackerListener<PatientModelService> > mServiceListener;
};

} // cx
#endif // CXPATIENTMODELSERVICEPROXY_H
