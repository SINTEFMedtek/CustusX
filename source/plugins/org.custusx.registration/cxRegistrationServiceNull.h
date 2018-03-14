/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
