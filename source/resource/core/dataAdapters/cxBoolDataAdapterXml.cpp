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


/*
 * sscBoolDataAdapterXml.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#include "cxBoolDataAdapterXml.h"
#include <iostream>
#include "cxTypeConversions.h"

namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
BoolDataAdapterXmlPtr BoolDataAdapterXml::initialize(const QString& uid, QString name, QString help, bool value,
	QDomNode root)
{
	BoolDataAdapterXmlPtr retval(new BoolDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(QString::number(value)).toInt();
	return retval;
}

QString BoolDataAdapterXml::getDisplayName() const
{
	return mName;
}

QString BoolDataAdapterXml::getValueAsString() const
{
	return mValue ? "true" : "false";
}

void BoolDataAdapterXml::setValueFromString(QString value)
{
	bool val = QString::compare(value, "true", Qt::CaseInsensitive) == 0 ? true : false;
	this->setValue(val);
}

QString BoolDataAdapterXml::getUid() const
{
	return mUid;
}

QString BoolDataAdapterXml::getHelp() const
{
	return mHelp;
}

void BoolDataAdapterXml::setHelp(QString val)
{
    mHelp = val;
    emit changed();
}

/*
bool BoolDataAdapterXml::getEnabled() const
{
	return mStore.readValue(QString::number(value)).toInt();
}

bool BoolDataAdapterXml::setEnabled(bool enabled)
{

}
*/

bool BoolDataAdapterXml::getValue() const
{
	return mValue;
}

bool BoolDataAdapterXml::setValue(bool val)
{
	if (val == mValue)
		return false;

	mValue = val;
	mStore.writeValue(QString::number(val));
	emit valueWasSet();
	emit changed();
	return true;
}

}
