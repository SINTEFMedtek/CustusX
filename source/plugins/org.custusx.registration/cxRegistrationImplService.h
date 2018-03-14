/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	virtual void addImage2ImageRegistration(Transform3D dMd, QString description);
	virtual void updateImage2ImageRegistration(Transform3D dMd, QString description);
	virtual void addPatientRegistration(Transform3D rMpr_new, QString description);
	virtual void updatePatientRegistration(Transform3D rMpr_new, QString description);
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
	virtual void updateRegistration_rMd(QDateTime oldTime, RegistrationTransform dMd, DataPtr data);
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
	void performImage2ImageRegistration(Transform3D dMd, QString description, bool temporaryRegistration = false);
	void performPatientRegistration(Transform3D rMpr_new, QString description, bool temporaryRegistration = false);
};

typedef boost::shared_ptr<RegistrationImplService> RegistrationImplServicePtr;

} /* namespace cx */

#endif /* CXREGISTRATIONIMPLSERVICE_H_ */

