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

#ifndef CXREGISTRATIONIMPLSERVICE_H_
#define CXREGISTRATIONIMPLSERVICE_H_

#include "cxRegistrationService.h"
#include <vector>
#include "org_custusx_registration_Export.h"
#include "qdatetime.h"
#include "cxLandmark.h"
#include "vtkForwardDeclarations.h"
class ctkPluginContext;
class QDomElement;

namespace cx
{
class RegistrationTransform;
class PatientModelService;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;


/**
 * Implementation of Registration service.
 *
 * \ingroup org_custusx_registration
 *
 *  \date 2014-08-26
 *  \author Ole Vegard Solberg, SINTEF
 *  \author Geir Arne Tangen, SINTEF
 */
class org_custusx_registration_EXPORT RegistrationImplService : public RegistrationService
{
	Q_INTERFACES(cx::RegistrationService)
public:
	RegistrationImplService(ctkPluginContext *context);
	virtual ~RegistrationImplService();

	virtual void setMovingData(DataPtr data);
	virtual void setFixedData(DataPtr data);
	void setMovingData(QString uid);
	void setFixedData(QString uid);
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

private slots:
	void duringSavePatientSlot(QDomElement &node);
	void duringLoadPatientSlot(QDomElement &node);
	void addXml(QDomNode &parentNode);
	void parseXml(QDomNode &dataNode);
	void clearSlot();
private:
	virtual void updateRegistration(QDateTime oldTime, RegistrationTransform deltaTransform, DataPtr data, bool continuous = false);
//	PatientModelService* getPatientModelService();
	void writePreLandmarkRegistration(QString name, LandmarkMap landmarks);
	vtkPointsPtr convertTovtkPoints(const std::vector<QString> &uids, const LandmarkMap &data, Transform3D M);
	std::vector<QString> getUsableLandmarks(const LandmarkMap &data_a, const LandmarkMap &data_b);
	Transform3D performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool *ok) const;
	std::vector<Vector3D> convertAndTransformToPoints(const std::vector<QString> &uids, const LandmarkMap &data, Transform3D M);
	std::vector<Vector3D> convertVtkPointsToPoints(vtkPointsPtr base);

//	DataPtr mFixedData; ///< the data that shouldn't update its matrices during a registrations
//	DataPtr mMovingData; ///< the data that should update its matrices during a registration
	QString mFixedData; ///< the data that shouldn't update its matrices during a registrations
	QString mMovingData; ///< the data that should update its matrices during a registration

	QDateTime mLastRegistrationTime; ///< last timestamp for registration during this session. All registrations in one session results in only one reg transform.

	ctkPluginContext* mContext;
	PatientModelServicePtr mPatientModelService;
	SessionStorageServicePtr mSession;
	void performImage2ImageRegistration(Transform3D delta_pre_rMd, QString description, bool continuous = false);
};

typedef boost::shared_ptr<RegistrationImplService> RegistrationImplServicePtr;

} /* namespace cx */

#endif /* CXREGISTRATIONIMPLSERVICE_H_ */

