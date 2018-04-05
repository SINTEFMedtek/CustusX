/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVector3DComponentProperty.h"

namespace cx
{

Vector3DComponentProperty::Vector3DComponentProperty(Vector3DPropertyBasePtr base, int index, QString name, QString help) :
		mBase(base), mIndex(index), mName(name), mHelp(help)
{
	connect(mBase.get(), SIGNAL(changed()), this, SIGNAL(changed()));
}

QString Vector3DComponentProperty::getDisplayName() const
{
	return mName.isEmpty() ? mBase->getDisplayName() : mName;
}

bool Vector3DComponentProperty::setValue(double value)
{
	Vector3D vec = mBase->getValue();
	vec[mIndex] = value;
//	std::cout << "set val for comp " << "  " << value << "  " << mIndex << std::endl;
	return mBase->setValue(vec);
}

double Vector3DComponentProperty::getValue() const
{
	return mBase->getValue()[mIndex];
}

QString Vector3DComponentProperty::getHelp() const
{
	return mHelp.isEmpty() ? mBase->getHelp() : mHelp;
}

DoubleRange Vector3DComponentProperty::getValueRange() const
{
	return mBase->getValueRange();
}

double Vector3DComponentProperty::convertInternal2Display(double internal)
{
	return mBase->convertInternal2Display(internal);
}

double Vector3DComponentProperty::convertDisplay2Internal(double display)
{
	return mBase->convertDisplay2Internal(display);
}

int Vector3DComponentProperty::getValueDecimals() const
{
	return mBase->getValueDecimals();
}



}
