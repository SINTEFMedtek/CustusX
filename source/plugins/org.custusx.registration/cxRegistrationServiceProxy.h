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

#ifndef CXREGISTRATIONSERVICEADAPTER_H
#define CXREGISTRATIONSERVICEADAPTER_H

#include "cxRegistrationService.h"
#include <QObject>
#include "cxServiceTrackerListener.h"
#include "cxTransform3D.h"
class QString;
class QDateTime;
class ctkPluginContext;
#include "org_custusx_registration_Export.h"

namespace cx
{

typedef boost::shared_ptr<class Data> DataPtr;

/** \brief Always provides a RegistrationService
 *
 * Use the Proxy design pattern.
 * Uses ServiceTrackerListener to either provide an
 * implementation of RegistrationService or
 * the null object (RegistrationServiceNull)
 *
 *  \ingroup org_custusx_registration
 *  \date 2014-09-10
 *  \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_EXPORT RegistrationServiceProxy : public RegistrationService
{
	Q_OBJECT
public:
	RegistrationServiceProxy(ctkPluginContext *context);
	~RegistrationServiceProxy() {}

	virtual void setMovingData(DataPtr data);
	virtual void setFixedData(DataPtr data);
	virtual DataPtr getMovingData();
	virtual DataPtr getFixedData();
	virtual void doPatientRegistration();
	virtual void doFastRegistration_Translation();
	virtual void doFastRegistration_Orientation(const Transform3D& tMtm, const Transform3D &prMt);
	virtual void doImageRegistration(bool translationOnly);
	virtual void applyImage2ImageRegistration(Transform3D delta_pre_rMd, QString description);
	virtual void applyContinuousImage2ImageRegistration(Transform3D delta_pre_rMd, QString description);
	virtual void applyPatientRegistration(Transform3D rMpr_new, QString description);
	virtual void applyPatientOrientation(const Transform3D &tMtm, const Transform3D &prMt);

	virtual QDateTime getLastRegistrationTime();
	virtual void setLastRegistrationTime(QDateTime time);
	virtual bool isNull();

//signals:
//	void fixedDataChanged(QString uid);
//	void movingDataChanged(QString uid);

private:
	void initServiceListener();
	void onServiceAdded(RegistrationService* service);
	void onServiceRemoved(RegistrationService *service);

	ctkPluginContext *mPluginContext;
	RegistrationServicePtr mRegistrationService;
	boost::shared_ptr<ServiceTrackerListener<RegistrationService> > mServiceListener;
};

} //cx
#endif // CXREGISTRATIONSERVICEADAPTER_H
