/*
 * cxVector3DComponentDataAdapter.cpp
 *
 *  Created on: Jul 25, 2011
 *      Author: christiana
 */

#include <cxVector3DComponentDataAdapter.h>

namespace cx
{

Vector3DComponentDataAdapter::Vector3DComponentDataAdapter(ssc::Vector3DDataAdapterPtr base, int index, QString name, QString help) :
		mBase(base), mIndex(index), mName(name), mHelp(help)
{
	connect(mBase.get(), SIGNAL(changed()), this, SIGNAL(changed()));
}

QString Vector3DComponentDataAdapter::getValueName() const
{
	return mName.isEmpty() ? mBase->getValueName() : mName;
}

bool Vector3DComponentDataAdapter::setValue(double value)
{
	ssc::Vector3D vec = mBase->getValue();
	vec[mIndex] = value;
//	std::cout << "set val for comp " << "  " << value << "  " << mIndex << std::endl;
	return mBase->setValue(vec);
}

double Vector3DComponentDataAdapter::getValue() const
{
	return mBase->getValue()[mIndex];
}

QString Vector3DComponentDataAdapter::getHelp() const
{
	return mHelp.isEmpty() ? mBase->getHelp() : mHelp;
}

ssc::DoubleRange Vector3DComponentDataAdapter::getValueRange() const
{
	return mBase->getValueRange();
}

double Vector3DComponentDataAdapter::convertInternal2Display(double internal)
{
	return mBase->convertInternal2Display(internal);
}

double Vector3DComponentDataAdapter::convertDisplay2Internal(double display)
{
	return mBase->convertDisplay2Internal(display);
}

int Vector3DComponentDataAdapter::getValueDecimals() const
{
	return mBase->getValueDecimals();
}



}
