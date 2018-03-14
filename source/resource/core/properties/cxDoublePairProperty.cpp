/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDoublePairProperty.h"
#include "cxTypeConversions.h"
#include "cxVector3D.h"

namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
DoublePairPropertyPtr DoublePairProperty::initialize(const QString& uid, QString name, QString help,
	DoubleRange range, int decimals, QDomNode root)
{
	DoublePairPropertyPtr retval(new DoublePairProperty());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = fromString(retval->mStore.readValue(qstring_cast(Eigen::Vector2d(0, 0))));
	retval->mDecimals = decimals;
	return retval;
}

DoublePairProperty::DoublePairProperty()
{
	mFactor = 1.0;
}

void DoublePairProperty::setInternal2Display(double factor)
{
	mFactor = factor;
}

QString DoublePairProperty::getDisplayName() const
{
	return mName;
}

QString DoublePairProperty::getUid() const
{
	return mUid;
}

QString DoublePairProperty::getHelp() const
{
	return mHelp;
}

Eigen::Vector2d DoublePairProperty::getValue() const
{
	return mValue;
}

bool DoublePairProperty::setValue(const Eigen::Vector2d& val)
{
	if (val == mValue)
		return false;

	mValue = val;
	mStore.writeValue(qstring_cast(val));
	emit valueWasSet();
	emit changed();
	return true;
}

DoubleRange DoublePairProperty::getValueRange() const
{
	return mRange;
}

void DoublePairProperty::setValueRange(DoubleRange range)
{
	mRange = range;
	emit changed();
}

int DoublePairProperty::getValueDecimals() const
{
	return mDecimals;
}

} // namespace cx
