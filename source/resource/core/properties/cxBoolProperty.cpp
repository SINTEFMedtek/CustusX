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
 * sscBoolProperty.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#include "cxBoolProperty.h"
#include <iostream>
#include "cxTypeConversions.h"

namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
BoolPropertyPtr BoolProperty::initialize(const QString& uid, QString name, QString help, bool value,
	QDomNode root)
{
	BoolPropertyPtr retval(new BoolProperty());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(QString::number(value)).toInt();
	return retval;
}

QString BoolProperty::getDisplayName() const
{
	return mName;
}


QString BoolProperty::getUid() const
{
	return mUid;
}

QString BoolProperty::getHelp() const
{
	return mHelp;
}

void BoolProperty::setHelp(QString val)
{
    mHelp = val;
    emit changed();
}

bool BoolProperty::getValue() const
{
	return mValue;
}

bool BoolProperty::setValue(bool val)
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
