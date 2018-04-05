/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxDoubleProperty.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>


namespace cx
{
DoubleProperty::DoubleProperty()
{
	mFactor = 1.0;
}

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
DoublePropertyPtr DoubleProperty::initialize(const QString& uid, QString name, QString help, double value,
	DoubleRange range, int decimals, QDomNode root)
{
	DoublePropertyPtr retval(new DoubleProperty());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(QString::number(value)).toDouble();
	retval->mDecimals = decimals;
	return retval;
}

void DoubleProperty::setInternal2Display(double factor)
{
	mFactor = factor;
}

QString DoubleProperty::getDisplayName() const
{
	return mName;
}

QString DoubleProperty::getUid() const
{
	return mUid;
}

QString DoubleProperty::getHelp() const
{
	return mHelp;
}

double DoubleProperty::getValue() const
{
	return mValue;
}

bool DoubleProperty::setValue(double val)
{
	if (val == mValue)
		return false;

	mValue = val;
	mStore.writeValue(QString::number(val));
    emit valueWasSet();
    emit changed();
	return true;
}

DoubleRange DoubleProperty::getValueRange() const
{
	return mRange;
}

void DoubleProperty::setValueRange(DoubleRange range)
{
	mRange = range;
	emit changed();
}

int DoubleProperty::getValueDecimals() const
{
	return mDecimals;
}

} // namespace cx
