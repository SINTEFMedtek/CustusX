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
DoubleDataAdapterXmlPtr DoubleDataAdapterXml::initialize(const QString& uid, QString name, QString help, double value,
	DoubleRange range, int decimals, QDomNode root)
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
	if (val == mValue)
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

void DoubleDataAdapterXml::setValueRange(DoubleRange range)
{
	mRange = range;
	emit changed();
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
