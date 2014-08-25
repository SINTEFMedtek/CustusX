/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


/*
 * sscVector3DDataAdapterXml.cpp
 *
 *  Created on: Jul 25, 2011
 *      Author: christiana
 */

#include "cxVector3DDataAdapterXml.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>
#include "cxTypeConversions.h"

namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
Vector3DDataAdapterXmlPtr Vector3DDataAdapterXml::initialize(const QString& uid, QString name, QString help,
	Vector3D value, DoubleRange range, int decimals, QDomNode root)
{
	Vector3DDataAdapterXmlPtr retval(new Vector3DDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = Vector3D::fromString(retval->mStore.readValue(qstring_cast(Vector3D(0, 0, 0))));
	retval->mDecimals = decimals;
	return retval;
}

Vector3DDataAdapterXml::Vector3DDataAdapterXml()
{
	mFactor = 1.0;
}

void Vector3DDataAdapterXml::setInternal2Display(double factor)
{
	mFactor = factor;
}

QString Vector3DDataAdapterXml::getDisplayName() const
{
	return mName;
}

QString Vector3DDataAdapterXml::getValueAsString() const
{
	QString val = prettyFormat(mValue, mDecimals);
	return val;
}

void Vector3DDataAdapterXml::setValueFromString(QString value)
{
	Vector3D val = Vector3D::fromString(value);
	this->setValue(val);
}

QString Vector3DDataAdapterXml::getUid() const
{
	return mUid;
}

QString Vector3DDataAdapterXml::getHelp() const
{
	return mHelp;
}

Vector3D Vector3DDataAdapterXml::getValue() const
{
	return mValue;
}

bool Vector3DDataAdapterXml::setValue(const Vector3D& val)
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

DoubleRange Vector3DDataAdapterXml::getValueRange() const
{
	return mRange;
}

void Vector3DDataAdapterXml::setValueRange(DoubleRange range)
{
	mRange = range;
	emit changed();
}

int Vector3DDataAdapterXml::getValueDecimals() const
{
	return mDecimals;
}

} // namespace cx
