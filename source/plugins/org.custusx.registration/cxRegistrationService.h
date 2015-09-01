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

#ifndef CXREGISTRATIONSERVICE_H
#define CXREGISTRATIONSERVICE_H

#include "cxResourceExport.h"
#include "org_custusx_registration_Export.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include "cxTransform3D.h"

class QDateTime;

namespace cx
{
typedef boost::shared_ptr<class Data> DataPtr;
}

#define RegistrationService_iid "cx::RegistrationService"

namespace cx
{
class RegistrationTransform;
typedef boost::shared_ptr<class RegistrationService> RegistrationServicePtr;

/** \brief Registration services
 *
 * This service replaces the old RegistrationManager class.
 *
 *  \ingroup org_custusx_registration
 *  \date 2014-08-26
 *  \author Ole Vegard Solberg, SINTEF
 *  \author Geir Arne Tangen, SINTEF
 */
class org_custusx_registration_EXPORT RegistrationService : public QObject
{
	Q_OBJECT
public:
	virtual ~RegistrationService() {}

	virtual void setMovingData(DataPtr data) = 0;
	virtual void setFixedData(DataPtr data) = 0;
	virtual DataPtr getMovingData() = 0;
	virtual DataPtr getFixedData() = 0;

	virtual void doPatientRegistration() = 0; ///< registrates the fixed image to the patient
	virtual void doFastRegistration_Translation() = 0; ///< use the landmarks in master image and patient to perform a translation-only landmark registration
	virtual void doFastRegistration_Orientation(const Transform3D& tMtm, const Transform3D &prMt) = 0;
	virtual void doImageRegistration(bool translationOnly) = 0;
	virtual void applyImage2ImageRegistration(Transform3D delta_pre_rMd, QString description) = 0;
	virtual void applyContinuousImage2ImageRegistration(Transform3D delta_pre_rMd, QString description) = 0;
	virtual void applyPatientRegistration(Transform3D rMpr_new, QString description) = 0;
	virtual void applyContinuousPatientRegistration(Transform3D rMpr_new, QString description) = 0;
	virtual void applyPatientOrientation(const Transform3D &tMtm, const Transform3D &prMt) = 0;

	virtual QDateTime getLastRegistrationTime() = 0;
	virtual void setLastRegistrationTime(QDateTime time) = 0;

	virtual bool isNull() = 0;
	static RegistrationServicePtr getNullObject();

	//Utility functions
	QString getFixedDataUid();
	QString getMovingDataUid();

signals:
	void fixedDataChanged(QString uid);
	void movingDataChanged(QString uid);
};

} //namespace cx
Q_DECLARE_INTERFACE(cx::RegistrationService, RegistrationService_iid)


#endif // CXREGISTRATIONSERVICE_H
