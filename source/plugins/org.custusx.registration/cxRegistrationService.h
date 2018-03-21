/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	virtual void addImage2ImageRegistration(Transform3D delta_pre_rMd, QString description) = 0;
	virtual void updateImage2ImageRegistration(Transform3D delta_pre_rMd, QString description) = 0;
	virtual void addPatientRegistration(Transform3D rMpr_new, QString description) = 0;
	virtual void updatePatientRegistration(Transform3D rMpr_new, QString description) = 0;
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
