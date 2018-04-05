/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationServiceNull.h"
#include "qdatetime.h"
#include "cxRegistrationTransform.h"


namespace cx
{

RegistrationServiceNull::RegistrationServiceNull()
{
}

void RegistrationServiceNull::setMovingData(DataPtr data)
{
	printWarning();
}

void RegistrationServiceNull::setFixedData(DataPtr data)
{
	printWarning();
}

DataPtr RegistrationServiceNull::getMovingData()
{
	return DataPtr();
}

DataPtr RegistrationServiceNull::getFixedData()
{
	return DataPtr();
}

void RegistrationServiceNull::doPatientRegistration()
{
	printWarning();
}

void RegistrationServiceNull::doFastRegistration_Translation()
{
	printWarning();
}

void RegistrationServiceNull::doFastRegistration_Orientation(const Transform3D &tMtm, const Transform3D &prMt)
{
	printWarning();
}

void RegistrationServiceNull::doImageRegistration(bool translationOnly)
{
	printWarning();
}

void RegistrationServiceNull::addImage2ImageRegistration(Transform3D delta_pre_rMd, QString description)
{
	printWarning();
}

void RegistrationServiceNull::updateImage2ImageRegistration(Transform3D delta_pre_rMd, QString description)
{
	printWarning();
}

void RegistrationServiceNull::addPatientRegistration(Transform3D rMpr_new, QString description)
{
	printWarning();
}

void RegistrationServiceNull::updatePatientRegistration(Transform3D rMpr_new, QString description)
{
	printWarning();
}

void RegistrationServiceNull::applyPatientOrientation(const Transform3D &tMtm, const Transform3D &prMt)
{
	printWarning();
}

QDateTime RegistrationServiceNull::getLastRegistrationTime()
{
	return QDateTime();
}

void RegistrationServiceNull::setLastRegistrationTime(QDateTime time)
{
	printWarning();
}

void RegistrationServiceNull::updateRegistration(QDateTime oldTime, RegistrationTransform deltaTransform, DataPtr data, QString masterFrame)
{
	printWarning();
}

bool RegistrationServiceNull::isNull()
{
	return true;
}

void RegistrationServiceNull::printWarning()
{
//	reportWarning("Trying to use RegistrationServiceNull. Is RegistrationService (org.custusx.registration) disabled?");
}

} // namespace cx
