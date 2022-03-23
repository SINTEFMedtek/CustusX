/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPatientModelServiceNull.h"
#include <QDomElement>
#include <map>
#include "cxImage.h"

#include "cxLandmark.h"
#include "cxRegistrationTransform.h"
#include "cxActiveData.h"

namespace cx
{

PatientModelServiceNull::PatientModelServiceNull()
{
}
void PatientModelServiceNull::insertData(DataPtr data, bool overWrite)
{
	printWarning();
}

DataPtr PatientModelServiceNull::createData(QString type, QString uid, QString name)
{
	return DataPtr();
}

std::map<QString, DataPtr> PatientModelServiceNull::getDatas(DataFilter filter) const
{
	printWarning();
	std::map<QString, DataPtr> retval;
	return retval;
}

std::map<QString, DataPtr> PatientModelServiceNull::getChildren(QString parent_uid, QString of_type) const
{
	printWarning();
	std::map<QString, DataPtr> retval;
	return retval;
}

LandmarksPtr PatientModelServiceNull::getPatientLandmarks() const
{
	printWarning();
	return boost::shared_ptr<Landmarks>();
}


std::map<QString, LandmarkProperty> PatientModelServiceNull::getLandmarkProperties() const
{
	printWarning();
	return std::map<QString, LandmarkProperty>();
}

void PatientModelServiceNull::setLandmarkName(QString uid, QString name)
{
	printWarning();
}

void PatientModelServiceNull::autoSave()
{
	printWarning();
}

bool PatientModelServiceNull::isNull()
{
	printWarning();
	return true;
}

void PatientModelServiceNull::makeAvailable(const QString &uid, bool available)
{
	printWarning();
}

std::map<QString, VideoSourcePtr> PatientModelServiceNull::getStreams() const
{
	printWarning();
	return std::map<QString, VideoSourcePtr>();
}

QString PatientModelServiceNull::getActivePatientFolder() const
{
	printWarning();
	return QString();
}

bool PatientModelServiceNull::isPatientValid() const
{
	printWarning();
	return false;
}

DataPtr PatientModelServiceNull::importData(QString fileName, QString &infoText)
{
	printWarning();
	return DataPtr();
}

void PatientModelServiceNull::exportPatient(PATIENT_COORDINATE_SYSTEM externalSpace)
{
	printWarning();
}

void PatientModelServiceNull::removeData(QString uid)
{
	printWarning();
}

PresetTransferFunctions3DPtr PatientModelServiceNull::getPresetTransferFunctions3D() const
{
	printWarning();
	return PresetTransferFunctions3DPtr();
}

void PatientModelServiceNull::setCenter(const Vector3D &center)
{
	printWarning();
}

Vector3D PatientModelServiceNull::getCenter() const
{
	return Vector3D::Zero();
}

void PatientModelServiceNull::setOperatingTable(const OperatingTable &ot)
{
	printWarning();
}

OperatingTable PatientModelServiceNull::getOperatingTable() const
{
	return OperatingTable(Transform3D::Identity());
}

QString PatientModelServiceNull::addLandmark()
{
	printWarning();
	return QString();
}

void PatientModelServiceNull::deleteLandmarks()
{
	printWarning();
}

void PatientModelServiceNull::setLandmarkActive(QString uid, bool active)
{
	printWarning();
}

void PatientModelServiceNull::printWarning() const
{
	//This warning is disabled as the null object is in normal use several places.
	//You can enable it e.g. when debugging.
	//	reportWarning("Trying to use PatientModelServiceNull. Is PatientModelService (org.custusx.patiemtmodel) disabled?");
}

RegistrationHistoryPtr PatientModelServiceNull::get_rMpr_History() const
{
	printWarning();
	return RegistrationHistory::getNullObject();
}

ActiveDataPtr PatientModelServiceNull::getActiveData() const
{
	printWarning();
	return ActiveData::getNullObject();
}

CLINICAL_VIEW PatientModelServiceNull::getClinicalApplication() const
{
	return mdCOUNT;
}

void PatientModelServiceNull::setClinicalApplication(CLINICAL_VIEW application)
{

}

} // cx
