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

#include "cxActiveData.h"
#include "cxImage.h"
#include "cxTrackedStream.h"
#include "cxPatientStorage.h"
#include "cxReporter.h"
#include "cxPatientModelService.h"

namespace cx
{

// --------------------------------------------------------
ActiveDataPtr ActiveData::mNull; ///< static member
// --------------------------------------------------------

/**NULL implementation of ActiveData
 */
class ActiveDataNull: public ActiveData
{
public:
	ActiveDataNull() :
		ActiveData(PatientModelService::getNullObject(), SessionStorageService::getNullObject()) {}
	virtual QList<DataPtr> getActiveDataHistory() const
	{
		printWarning("getActiveDataHistory");
		return QList<DataPtr>();
	}
	virtual DataPtr getActive() const
	{
		printWarning("getActive");
		return DataPtr();
	}
	virtual DataPtr getActiveUsingRegexp(QString typeRegexp) const
	{
		printWarning("getActiveUsingRegexp(QString)");
		return DataPtr();
	}
	virtual ImagePtr getDerivedActiveImage() const
	{
		printWarning("getDerivedActiveImage");
		return ImagePtr();
	}
	virtual void setActive(DataPtr activeData)
	{
		printWarning("setActive");
	}
	virtual QString getActiveImageUid()
	{
		printWarning("getActiveImageUid");
		return QString();
	}
	virtual void remove(DataPtr dataToBeRemoved)
	{
		printWarning("remove");
	}
	void printWarning(QString warning = "") const
	{
		reportWarning("Trying to use ActiveDataNull. Function: " + warning);
	}
	static ActiveDataPtr getNullObject();
	virtual bool isNull() const
	{
		return true;
	}
};

//---------------------------------------------------------
//-------  ActiveData  ------------------------------------
//---------------------------------------------------------


ActiveData::ActiveData(PatientModelServicePtr patientModelService, SessionStorageServicePtr sessionStorageService) :
	mStorage(new PatientStorage(sessionStorageService, "ActiveData", true)),
	mPatientModelService(patientModelService)
{
	mStorage->storeVariable("activeUids",
							boost::bind(&ActiveData::getStringToSave, this),
							boost::bind(&ActiveData::loadFromString, this, _1));
}

QString ActiveData::getStringToSave() const
{
	return this->getActiveDataStringList().join(" ");
}

QStringList ActiveData::getActiveDataStringList() const
{
	QStringList retval;
	if(!mActiveData.isEmpty())
		for(int i = 0; i < mActiveData.size(); ++i)
			retval << mActiveData.at(i)->getUid();
	return retval;
}

void ActiveData::loadFromString(const QString activeDatas)
{
	mActiveData.clear();
	QStringList activeDataList = activeDatas.split(" ");
	for(int i = 0; i < activeDataList.size(); ++i)
	{
		DataPtr data = mPatientModelService->getData(activeDataList.at(i));
		this->setActive(data);
	}
}

QList<DataPtr> ActiveData::getActiveDataHistory() const
{
	return mActiveData;
}

DataPtr ActiveData::getActive() const
{
	if(mActiveData.isEmpty())
		return DataPtr();
	return mActiveData.last();
}

DataPtr ActiveData::getActiveUsingRegexp(QString typeRegexp) const
{
	QList<DataPtr> activeDatas = getActiveDataHistory(typeRegexp);

	DataPtr activeData;
	if(!activeDatas.isEmpty())
		activeData = activeDatas.last();

	return activeData;
}

ImagePtr ActiveData::getDerivedActiveImage() const
{
	DataPtr activeData = this->getActiveUsingRegexp("image|trackedStream");
	ImagePtr retval;
	TrackedStreamPtr stream = boost::dynamic_pointer_cast<TrackedStream>(activeData);
	if(stream)
		retval = stream->getChangingImage();
	else
		retval = boost::dynamic_pointer_cast<Image>(activeData);
	return retval;
}

QList<DataPtr> ActiveData::getActiveDataHistory(QString typeRegexp) const
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

void ActiveData::setActive(DataPtr activeData)
{
	if (!activeData)
		return;

	mActiveData.removeAll(activeData);
	mActiveData.append(activeData);

	this->emitSignals(activeData);
}

QString ActiveData::getActiveImageUid()
{
	ImagePtr image = this->getActive<Image>();
	if (image)
		return image->getUid();
	else
		return "";
}

void ActiveData::emitSignals(DataPtr activeData)
{
	this->emitActiveDataChanged();
	if(activeData && activeData->getType() == "image")
		this->emitActiveImageChanged();
}

void ActiveData::emitActiveImageChanged()
{
	DataPtr activeData = ActiveData::getActive<Image>();
	QString uid = getChangedUid(activeData);
	emit activeImageChanged(uid);
}

void ActiveData::emitActiveDataChanged()
{
	DataPtr activeData = this->getActive();
	QString uid = getChangedUid(activeData);
	emit activeDataChanged(uid);
}

QString ActiveData::getChangedUid(DataPtr activeData) const
{
	QString uid = "";
	if(activeData)
		uid = activeData->getUid();
	return uid;
}

void ActiveData::remove(DataPtr dataToBeRemoved)
{
	if(!dataToBeRemoved)
		reportWarning("ActiveData::remove: No data");
	if(!mActiveData.contains(dataToBeRemoved))
		return;

	bool resendActiveImage = false;
	bool resendActiveData = false;
	if (this->getActive<Image>() == dataToBeRemoved)
		resendActiveImage = true;
	if(this->getActive() == dataToBeRemoved)
		resendActiveData = true;

	mActiveData.removeAll(dataToBeRemoved);

	if(resendActiveImage)
		emitActiveImageChanged();
	if(resendActiveData)
		emitActiveDataChanged();
}

ActiveDataPtr ActiveData::getNullObject()
{
	if (!mNull)
		mNull.reset(new ActiveDataNull);
	return mNull;
}

}//cx
