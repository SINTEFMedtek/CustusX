/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPATIENTMODELSERVICENULL_H
#define CXPATIENTMODELSERVICENULL_H

#include "cxResourceExport.h"
#include "cxPatientModelService.h"

namespace cx
{

/** \brief Null Object Pattern for Registration service
 *
 *
 *  \ingroup cx_resource_core_data
 *  \date 2014-09-10
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT PatientModelServiceNull : public PatientModelService
{
public:
	PatientModelServiceNull();
	virtual void insertData(DataPtr data, bool overWrite = false);
	virtual DataPtr createData(QString type, QString uid, QString name="");
	virtual std::map<QString, DataPtr> getDatas(DataFilter filter) const;
	virtual std::map<QString, DataPtr> getChildren(QString parent_uid, QString of_type="") const;

	virtual LandmarksPtr getPatientLandmarks() const;
	virtual std::map<QString, LandmarkProperty> getLandmarkProperties() const;
	virtual void setLandmarkName(QString uid, QString name);
	virtual void setLandmarkActive(QString uid, bool active);

	virtual RegistrationHistoryPtr get_rMpr_History() const;

	virtual ActiveDataPtr getActiveData() const;

	virtual CLINICAL_VIEW getClinicalApplication() const;
	virtual void setClinicalApplication(CLINICAL_VIEW application);

	virtual std::map<QString, cx::VideoSourcePtr> getStreams() const;

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

private:
	void printWarning() const;
};

} //cx
#endif // CXPATIENTMODELSERVICENULL_H
