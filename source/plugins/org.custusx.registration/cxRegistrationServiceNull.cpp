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

void RegistrationServiceNull::applyImage2ImageRegistration(Transform3D delta_pre_rMd, QString description)
{
	printWarning();
}

void RegistrationServiceNull::applyContinuousImage2ImageRegistration(Transform3D delta_pre_rMd, QString description)
{
	printWarning();
}

void RegistrationServiceNull::applyPatientRegistration(Transform3D rMpr_new, QString description)
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
