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
void PatientModelServiceNull::insertData(DataPtr data)
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
	printWarning();;
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

void PatientModelServiceNull::setLandmarkActive(QString uid, bool active)
{
	printWarning();
}

void PatientModelServiceNull::printWarning() const
{
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
