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

#include "cxSelectDataStringPropertyBase.h"
#include "cxPatientModelService.h"
#include "cxData.h"
#include "cxImageAlgorithms.h"

namespace cx
{

SelectDataStringPropertyBase::SelectDataStringPropertyBase(PatientModelServicePtr patientModelService, QString typeRegexp) :
	mTypeRegexp(typeRegexp),
	mPatientModelService(patientModelService)
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

void SelectDataStringPropertyBase::setUidRegexp(QString regexp)
{
	mUidRegexp = regexp;
}

QStringList SelectDataStringPropertyBase::getValueRange() const
{
	std::map<QString, DataPtr> data = mPatientModelService->getData();
	data = SelectDataStringPropertyBase::filterOnType(data, mTypeRegexp);
	data = this->filterOnUid(data, mUidRegexp);
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
