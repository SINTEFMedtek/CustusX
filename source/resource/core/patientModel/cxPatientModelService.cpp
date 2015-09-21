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
	//TODO: Should active be remaned to activeImage
	if (uid=="active")
		return this->getActiveData<Image>();

	std::map<QString, DataPtr> all = this->getData();
	std::map<QString, DataPtr>::const_iterator iter = all.find(uid);
	if (iter == all.end())
		return DataPtr();
	return iter->second;
}

QString PatientModelService::getActiveImageUid()
{
	ImagePtr image = this->getActiveData<Image>();
	if (image)
		return image->getUid();
	else
		return "";
}

Transform3D PatientModelService::get_rMpr() const
{
	return this->get_rMpr_History()->getCurrentRegistration().mValue;
}

DataPtr PatientModelService::getActiveData() const
{
	if(this->getActiveDataHistory().isEmpty())
		return DataPtr();
	return this->getActiveDataHistory().last();
}

QList<DataPtr> PatientModelService::getActiveDataHistory(QString typeRegexp) const
{
	QRegExp reg(typeRegexp);
	QList<DataPtr> activeDatas = this->getActiveDataHistory();

	for(int i = 0; i < activeDatas.size(); )
	{
		int current = i++;
		if(!activeDatas.at(current)->getType().contains(reg))
			activeDatas.removeAt(current);
	}

	return activeDatas;
}

DataPtr PatientModelService::getActiveData(QString typeRegexp) const
{
	QList<DataPtr> activeDatas = getActiveDataHistory(typeRegexp);

	DataPtr activeData;
	if(!activeDatas.isEmpty())
		activeData = activeDatas.last();

	return activeData;
}

ImagePtr PatientModelService::getDerivedActiveImage() const
{
	DataPtr activeData = this->getActiveData("image|trackedStream");
	ImagePtr retval;
	TrackedStreamPtr stream = boost::dynamic_pointer_cast<TrackedStream>(activeData);
	if(stream)
		retval = stream->getChangingImage();
	else
		retval = boost::dynamic_pointer_cast<Image>(activeData);
	return retval;
}

void PatientModelService::setActiveData(QString uid)
{
	this->setActiveData(this->getData(uid));
}

void PatientModelService::updateRegistration_rMpr(const QDateTime& oldTime, const RegistrationTransform& newTransform, bool continuous)
{
	//Block signals from RegistrationHistory when running continuous registration,
	//because these trigger RegistrationHistoryWidget::prePaintEvent() that uses too much time.
	this->get_rMpr_History()->blockSignals(continuous);

	this->get_rMpr_History()->updateRegistration(oldTime, newTransform);
	if(!continuous)
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
