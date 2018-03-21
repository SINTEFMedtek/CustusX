/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSelectDataStringPropertyBase.h"
#include "cxPatientModelService.h"
#include "cxData.h"
#include "cxImageAlgorithms.h"
#include "cxImage.h"

namespace cx
{

SelectDataStringPropertyBase::SelectDataStringPropertyBase(PatientModelServicePtr patientModelService, QString typeRegexp) :
	mTypeRegexp(typeRegexp),
	mPatientModelService(patientModelService),
	mOnly2D(false)
{
	mValueName = "Select data";
	mUidRegexp = "";
	mHelp = mValueName;
	connect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(changed()));
}

/**
	* Erase all data with type not conforming to input regexp.
	*/
std::map<QString, DataPtr> SelectDataStringPropertyBase::filterOnType(std::map<QString, DataPtr> input, QString regexp)
{
	QRegExp reg(regexp);

	std::map<QString, DataPtr>::iterator iter, current;

	for (iter=input.begin(); iter!=input.end(); )
	{
		current = iter++; // increment iterator before erasing!
		if (!current->second->getType().contains(reg))
			input.erase(current);
	}

	return input;
}

void SelectDataStringPropertyBase::setTypeRegexp(QString regexp)
{
	mTypeRegexp = regexp;
	emit changed();
}

/**
	* Erase all data with uid not conforming to input regexp.
	*/
std::map<QString, DataPtr> SelectDataStringPropertyBase::filterOnUid(std::map<QString, DataPtr> input, QString regexp) const
{

	QRegExp reg(regexp);

	std::map<QString, DataPtr>::iterator iter, current;
	for (iter=input.begin(); iter!=input.end(); )
	{
		current = iter++; // increment iterator before erasing!
		if (!current->second->getUid().contains(reg))
			input.erase(current);
	}

	return input;
}

std::map<QString, DataPtr> SelectDataStringPropertyBase::filterImagesOn2D(std::map<QString, DataPtr> input, bool only2D) const
{
	if(!only2D)
		return input;

	std::map<QString, DataPtr>::iterator iter, current;
	for (iter=input.begin(); iter!=input.end(); )
	{
		current = iter++; // increment iterator before erasing!
		ImagePtr image = boost::dynamic_pointer_cast<Image>(current->second);
		if(image && !image->is2D())
			input.erase(current);
	}

	return input;
}

void SelectDataStringPropertyBase::setOnly2DImagesFilter(bool only2D)
{
	mOnly2D = only2D;
	emit changed();
}

void SelectDataStringPropertyBase::setUidRegexp(QString regexp)
{
	mUidRegexp = regexp;
	emit changed();
}

QStringList SelectDataStringPropertyBase::getValueRange() const
{
	std::map<QString, DataPtr> data = mPatientModelService->getDatas();
	data = SelectDataStringPropertyBase::filterOnType(data, mTypeRegexp);
	data = this->filterOnUid(data, mUidRegexp);
	data = this->filterImagesOn2D(data, mOnly2D);
	std::vector<DataPtr> sorted = sortOnGroupsAndAcquisitionTime(data);
	QStringList retval;
	retval << "";
	for (unsigned i=0; i<sorted.size(); ++i)
	retval << sorted[i]->getUid();
	return retval;
}

QString SelectDataStringPropertyBase::convertInternal2Display(QString internal)
{
	DataPtr data = mPatientModelService->getData(internal);
	if (!data)
	return "<no data>";
	return qstring_cast(data->getName());
}

QString SelectDataStringPropertyBase::getHelp() const
{
	return mHelp;
}

QString SelectDataStringPropertyBase::getDisplayName() const
{
	return mValueName;
}

void SelectDataStringPropertyBase::setHelp(QString text)
{
	mHelp = text;
	emit changed();
}
void SelectDataStringPropertyBase::setValueName(QString val)
{
	mValueName = val;
	emit changed();
}

DataPtr SelectDataStringPropertyBase::getData() const
{
	return mPatientModelService->getData(this->getValue());
}

DataPtr SelectDataStringPropertyBase::getData(QString uid) const
{
	return mPatientModelService->getData(uid);
}

} //cx
