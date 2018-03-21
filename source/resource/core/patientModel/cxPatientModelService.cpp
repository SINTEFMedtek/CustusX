/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPatientModelService.h"
#include "cxPatientModelServiceNull.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxTrackedStream.h"
#include "cxRegistrationTransform.h"
#include <QDir>
#include "cxTime.h"
#include "cxReporter.h"
#include "cxActiveData.h"

namespace cx
{
PatientModelServicePtr PatientModelService::getNullObject()
{
	static PatientModelServicePtr mNull;
	if (!mNull)
		mNull.reset(new PatientModelServiceNull);
	return mNull;
}

DataPtr PatientModelService::getData(const QString& uid) const
{
	if (uid=="active")
		return this->getActiveData()->getActive();

	std::map<QString, DataPtr> all = this->getDatas(AllData);
	std::map<QString, DataPtr>::const_iterator iter = all.find(uid);
	if (iter == all.end())
		return DataPtr();
	return iter->second;
}

Transform3D PatientModelService::get_rMpr() const
{
	return this->get_rMpr_History()->getCurrentRegistration().mValue;
}

void PatientModelService::updateRegistration_rMpr(const QDateTime& oldTime, const RegistrationTransform& newTransform)
{
	this->get_rMpr_History()->addOrUpdateRegistration(oldTime, newTransform);

	if(!newTransform.mTemp)
		this->autoSave();
}

VideoSourcePtr PatientModelService::getStream(const QString& uid) const
{
	std::map<QString, VideoSourcePtr> streams = this->getStreams();
	if (streams.count(uid))
		return streams.find(uid)->second;
	return VideoSourcePtr();
}

QString PatientModelService::generateFilePath(QString folderName, QString ending)
{
	QString folder = this->getActivePatientFolder() + "/" +folderName + "/";
	QDir().mkpath(folder);
	QString format = timestampSecondsFormat();
	QString filename = QDateTime::currentDateTime().toString(format) + "." + ending;

	return folder+filename;
}

} //cx
