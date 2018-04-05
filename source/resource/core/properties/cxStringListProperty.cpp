/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStringListProperty.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>
#include "cxTypeConversions.h"

namespace cx
{

StringListProperty::StringListProperty()
{}

StringListPropertyPtr StringListProperty::initialize(const QString& uid, QString name, QString help, QStringList value, QStringList range, QDomNode root)
{
	StringListPropertyPtr retval(new StringListProperty());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readVariant(value).value<QStringList>();
	retval->mRange = range;
	return retval;
}

void StringListProperty::setDisplayName(QString val)
{
	mName = val;
	emit changed();
}

QString StringListProperty::getDisplayName() const
{
	return mName;
}

QString StringListProperty::getUid() const
{
	return mUid;
}

QString StringListProperty::getHelp() const
{
	return mHelp;
}

void StringListProperty::setHelp(QString val)
{
	if (val == mHelp)
		return;

	mHelp = val;
	emit changed();
}

QVariant StringListProperty::getValueAsVariant() const
{
	return this->getValue();
}

void StringListProperty::setValueFromVariant(QVariant val)
{
	this->setValue(val.value<QStringList>());
}


QStringList StringListProperty::getValue() const
{
	return mValue;
}

bool StringListProperty::setValue(const QStringList& val)
{
	if (val == mValue)
		return false;

	mValue.clear();
	for (int i=0; i<val.size(); ++i)
		if (mRange.contains(val[i]))
			mValue.push_back(val[i]);

	mStore.writeVariant(mValue);
	emit changed();
	return true;
}

QStringList StringListProperty::getValueRange() const
{
	return mRange;
}

void StringListProperty::setValueRange(QStringList range)
{
	mRange = range;
	emit changed();
}

/**If a mapping from internal name to display name has been set, use it.
 * Otherwise return the input.
 */
QString StringListProperty::convertInternal2Display(QString internal)
{
	if (mDisplayNames.count(internal))
		return mDisplayNames[internal];
	return internal;
}

void StringListProperty::setDisplayNames(std::map<QString, QString> names)
{
	mDisplayNames = names;
	emit changed();
}

} // namespace cx

