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
 * sscStringDataAdapterXml.cpp
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */
#include "sscStringDataAdapterXml.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>

namespace ssc
{

StringDataAdapterXml::StringDataAdapterXml() : 	mIsReadOnly(false)
{

}

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
StringDataAdapterXmlPtr StringDataAdapterXml::initialize(const QString& uid, QString name, QString help, QString value,
	QStringList range, QDomNode root)
{
	StringDataAdapterXmlPtr retval(new StringDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(value);
	retval->mAllowOnlyValuesInRange = true;
	return retval;
}

StringDataAdapterXmlPtr StringDataAdapterXml::initialize(const QString& uid, QString name, QString help, QString value, QDomNode root)
{
	StringDataAdapterXmlPtr retval(new StringDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	//retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(value);
	retval->mAllowOnlyValuesInRange = false;
	return retval;
}

void StringDataAdapterXml::setReadOnly(bool val)
{
	mIsReadOnly = val;
	emit changed();
}

QString StringDataAdapterXml::getUid() const
{
	return mUid;
}

QString StringDataAdapterXml::getValueName() const
{
	return mName;
}

QString StringDataAdapterXml::getHelp() const
{
	return mHelp;
}

void StringDataAdapterXml::setHelp(QString val)
{
    if (val == mHelp)
        return;

    mHelp = val;
    emit changed();
}


QString StringDataAdapterXml::getValue() const
{
	return mValue;
}

bool StringDataAdapterXml::setValue(const QString& val)
{
	if (val == mValue)
		return false;

	mValue = val;
	mStore.writeValue(val);
	emit valueWasSet();
	emit changed();
	return true;
}

QStringList StringDataAdapterXml::getValueRange() const
{
	return mRange;
}

void StringDataAdapterXml::setValueRange(QStringList range)
{
	mRange = range;
	emit changed();
}

/**If a mapping from internal name to display name has been set, use it.
 * Otherwise return the input.
 */
QString StringDataAdapterXml::convertInternal2Display(QString internal)
{
	if (mDisplayNames.count(internal))
		return mDisplayNames[internal];
	return internal;
}

void StringDataAdapterXml::setDisplayNames(std::map<QString, QString> names)
{
	mDisplayNames = names;
}

} // namespace ssc
