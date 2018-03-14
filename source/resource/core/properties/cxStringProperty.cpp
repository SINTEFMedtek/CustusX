/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStringProperty.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>
#include "cxTypeConversions.h"

namespace cx
{

StringProperty::StringProperty() :
    mIsReadOnly(false)
{}

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
StringPropertyPtr StringProperty::initialize(const QString& uid, QString name, QString help, QString value, QStringList range, QDomNode root)
{
    StringPropertyPtr retval = initialize(uid, name, help, value, root);
    retval->mRange = range;
	retval->mAllowOnlyValuesInRange = true;
	return retval;
}

StringPropertyPtr StringProperty::initialize(const QString& uid, QString name, QString help, QString value, QDomNode root)
{
	StringPropertyPtr retval(new StringProperty());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(value);
	retval->mAllowOnlyValuesInRange = false;
	return retval;
}

void StringProperty::setReadOnly(bool val)
{
	mIsReadOnly = val;
	emit changed();
}

void StringProperty::setDisplayName(QString val)
{
	mName = val;
	emit changed();
}


QString StringProperty::getDisplayName() const
{
	return mName;
}

QString StringProperty::getUid() const
{
	return mUid;
}

QString StringProperty::getHelp() const
{
	return mHelp;
}

void StringProperty::setHelp(QString val)
{
    if (val == mHelp)
        return;

    mHelp = val;
    emit changed();
}


QString StringProperty::getValue() const
{
	return mValue;
}

bool StringProperty::setValue(const QString& val)
{
	if (val == mValue)
		return false;

	mValue = val;
	mStore.writeValue(val);
	emit valueWasSet();
	emit changed();
	return true;
}

QStringList StringProperty::getValueRange() const
{
	return mRange;
}

void StringProperty::setValueRange(QStringList range)
{
	mRange = range;
	emit changed();
}

/**If a mapping from internal name to display name has been set, use it.
 * Otherwise return the input.
 */
QString StringProperty::convertInternal2Display(QString internal)
{
	if (mDisplayNames.count(internal))
		return mDisplayNames[internal];
	return internal;
}

void StringProperty::setDisplayNames(std::map<QString, QString> names)
{
	mDisplayNames = names;
	emit changed();
}

} // namespace cx
