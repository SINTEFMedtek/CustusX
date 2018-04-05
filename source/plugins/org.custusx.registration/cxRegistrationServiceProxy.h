/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	virtual void addImage2ImageRegistration(Transform3D delta_pre_rMd, QString description);
	virtual void updateImage2ImageRegistration(Transform3D delta_pre_rMd, QString description);
	virtual void addPatientRegistration(Transform3D rMpr_new, QString description);
	virtual void updatePatientRegistration(Transform3D rMpr_new, QString description);
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
