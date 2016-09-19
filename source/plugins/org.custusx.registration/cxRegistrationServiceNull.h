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
#ifndef CXREGISTRATIONSERVICENULL_H
#define CXREGISTRATIONSERVICENULL_H

#include "cxResourceExport.h"

#include "cxRegistrationService.h"

namespace cx
{

/** \brief Null Object Pattern for Registration service
 *
 *
 *  \ingroup org_custusx_registration
 *  \date 2014-08-28
 *  \author Ole Vegard Solberg, SINTEF
 */
class RegistrationServiceNull : public RegistrationService
{

public:
	RegistrationServiceNull();
	~RegistrationServiceNull() {}
	virtual void setMovingData(DataPtr data);
	virtual void setFixedData(DataPtr data);
	virtual DataPtr getMovingData();
	virtual DataPtr getFixedData();

	virtual void doPatientRegistration();
	virtual void doFastRegistration_Translation();
	virtual void doFastRegistration_Orientation(const Transform3D& tMtm, const Transform3D &prMt);
	virtual void doImageRegistration(bool translationOnly);
	virtual void addImage2ImageRegistration(Transform3D delta_pre_rMd, QString description);
	virtual void updateImage2ImageRegistration(Transform3D delta_pre_rMd, QString description);
	virtual void addPatientRegistration(Transform3D rMpr_new, QString description);
	virtual void updatePatientRegistration(Transform3D rMpr_new, QString description);
	virtual void applyPatientOrientation(const Transform3D &tMtm, const Transform3D &prMt);

	virtual QDateTime getLastRegistrationTime();
	virtual void setLastRegistrationTime(QDateTime time);

	virtual void updateRegistration(QDateTime oldTime, RegistrationTransform deltaTransform, DataPtr data, QString masterFrame);

	virtual bool isNull();
private:
	void printWarning();
};
} // namespace cx
#endif // CXREGISTRATIONSERVICENULL_H
