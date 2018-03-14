/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
//		reportWarning("Trying to use ActiveDataNull. Function: " + warning);
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
	QList<DataPtr> active = this->getActiveDataHistory();
	QList<DataPtr> retval;

	for(int i = 0; i < active.size(); ++i)
	{
		DataPtr current = active[i];
		if(current->getType().contains(reg))
			retval.push_back(current);
	}

	return retval;
}

void ActiveData::setActive(DataPtr activeData)
{
	if (!activeData)
		return;
	if (!mActiveData.empty() && mActiveData.last() == activeData)
		return;

	mActiveData.removeAll(activeData);
	mActiveData.append(activeData);

	this->emitSignals(activeData);
}

void ActiveData::setActive(QString uid)
{
	DataPtr dataToSet = mPatientModelService->getData(uid);
	this->setActive(dataToSet);
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
