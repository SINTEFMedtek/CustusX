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

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
StringDataAdapterXmlPtr StringDataAdapterXml::initialize(const QString& uid,
    QString name,
    QString help,
    QString value,
    QStringList range,
    QDomNode root)
{
	StringDataAdapterXmlPtr retval(new StringDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(value);
	return retval;
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

QString StringDataAdapterXml::getValue() const
{
  return mValue;
}

bool StringDataAdapterXml::setValue(const QString& val)
{
	if (val==mValue)
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



} // namespace ssc
