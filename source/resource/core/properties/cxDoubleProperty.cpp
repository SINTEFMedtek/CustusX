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


#include "cxDoubleProperty.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>


namespace cx
{
DoubleDataAdapterXml::DoubleDataAdapterXml()
{
	mFactor = 1.0;
}

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

QString DoubleDataAdapterXml::getDisplayName() const
{
	return mName;
}

QString DoubleDataAdapterXml::getUid() const
{
	return mUid;
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

} // namespace cx
