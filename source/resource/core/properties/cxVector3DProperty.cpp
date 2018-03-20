/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVector3DProperty.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>
#include "cxTypeConversions.h"

namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
Vector3DPropertyPtr Vector3DProperty::initialize(const QString& uid, QString name, QString help,
	Vector3D value, DoubleRange range, int decimals, QDomNode root)
{
	Vector3DPropertyPtr retval(new Vector3DProperty());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = Vector3D::fromString(retval->mStore.readValue(qstring_cast(Vector3D(0, 0, 0))));
	retval->mDecimals = decimals;
	return retval;
}

Vector3DProperty::Vector3DProperty()
{
	mFactor = 1.0;
}

void Vector3DProperty::setInternal2Display(double factor)
{
	mFactor = factor;
}

QString Vector3DProperty::getDisplayName() const
{
	return mName;
}

QString Vector3DProperty::getUid() const
{
	return mUid;
}

QString Vector3DProperty::getHelp() const
{
	return mHelp;
}

Vector3D Vector3DProperty::getValue() const
{
	return mValue;
}

bool Vector3DProperty::setValue(const Vector3D& val)
{
	if (similar(val, mValue))
		return false;

	//	std::cout << "set val " << "  " << val << "  , org=" << mValue << std::endl;

	mValue = val;
	mStore.writeValue(qstring_cast(val));
	emit
	valueWasSet();
	emit
	changed();
	return true;
}

DoubleRange Vector3DProperty::getValueRange() const
{
	return mRange;
}

void Vector3DProperty::setValueRange(DoubleRange range)
{
	mRange = range;
	emit changed();
}

int Vector3DProperty::getValueDecimals() const
{
	return mDecimals;
}

} // namespace cx
