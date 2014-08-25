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


#include "cxColorDataAdapterXml.h"
#include "cxVector3D.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"

namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
ColorDataAdapterXmlPtr ColorDataAdapterXml::initialize(const QString& uid, QString name, QString help, QColor value,
    QDomNode root)
{
    ColorDataAdapterXmlPtr retval(new ColorDataAdapterXml());
    retval->mUid = uid;
    retval->mName = name.isEmpty() ? uid : name;
    retval->mHelp = help;
    retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = string2color(retval->mStore.readValue(color2string(value)));
    return retval;
}

QString ColorDataAdapterXml::getDisplayName() const
{
    return mName;
}

QString ColorDataAdapterXml::getValueAsString() const
{
	return mValue.name();
}

void ColorDataAdapterXml::setValueFromString(QString value)
{
	QColor val(value);
	if(val.isValid())
		this->setValue(val);
	else
		reportError("Could not convert "+value+" to QColor.");
}

QString ColorDataAdapterXml::getUid() const
{
    return mUid;
}

QString ColorDataAdapterXml::getHelp() const
{
    return mHelp;
}

QColor ColorDataAdapterXml::getValue() const
{
    return mValue;
}

bool ColorDataAdapterXml::setValue(QColor val)
{
    if (val == mValue)
        return false;

    mValue = val;
	mStore.writeValue(color2string(val));
    emit valueWasSet();
    emit changed();
    return true;
}

} // namespace cx

