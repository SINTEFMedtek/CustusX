// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscBoolDataAdapterXml.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#include <sscBoolDataAdapterXml.h>
#include <iostream>
#include "sscTypeConversions.h"

namespace ssc
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

QString BoolDataAdapterXml::getUid() const
{
	return mUid;
}

QString BoolDataAdapterXml::getValueName() const
{
	return mName;
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
