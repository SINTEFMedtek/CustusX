/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
