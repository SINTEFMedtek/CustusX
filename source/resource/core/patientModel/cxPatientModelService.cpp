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
	this->get_rMpr_History()->addRegistration(oldTime, newTransform);

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
