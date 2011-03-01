/*
 * sscDoubleDataAdapterXml.cpp
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */
#include "sscDoubleDataAdapterXml.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>

namespace ssc
{


/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
DoubleDataAdapterXmlPtr DoubleDataAdapterXml::initialize(const QString& uid,
    QString name,
    QString help,
    double value,
    DoubleRange range,
    int decimals,
    QDomNode root)
{
	DoubleDataAdapterXmlPtr retval(new DoubleDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(QString::number(value)).toDouble();
	retval->mDecimals = decimals;
	return retval;
}

void DoubleDataAdapterXml::setInternal2Display(double factor)
{
  mFactor = factor;
}

QString DoubleDataAdapterXml::getUid() const
{
	return mUid;
}

QString DoubleDataAdapterXml::getValueName() const
{
	return mName;
}

QString DoubleDataAdapterXml::getHelp() const
{
	return mHelp;
}

double DoubleDataAdapterXml::getValue() const
{
  return mValue;
}

bool DoubleDataAdapterXml::setValue(double val)
{
	if (val==mValue)
		return false;

	mValue = val;
	mStore.writeValue(QString::number(val));
	emit valueWasSet();
	emit changed();
	return true;
}

DoubleRange DoubleDataAdapterXml::getValueRange() const
{
	return mRange;
}

int DoubleDataAdapterXml::getValueDecimals() const
{
	return mDecimals;
}

DoubleDataAdapterXml::DoubleDataAdapterXml()
{
  mFactor = 1.0;
}

} // namespace ssc
