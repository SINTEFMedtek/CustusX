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

#include "cxStringListProperty.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>
#include "cxTypeConversions.h"

namespace cx
{

StringListProperty::StringListProperty()
{}

StringListPropertyPtr StringListProperty::initialize(const QString& uid, QString name, QString help, QStringList value, QStringList range, QDomNode root)
{
	StringListPropertyPtr retval(new StringListProperty());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readVariant(value).value<QStringList>();
	retval->mRange = range;
	return retval;
}

void StringListProperty::setDisplayName(QString val)
{
	mName = val;
	emit changed();
}

QString StringListProperty::getDisplayName() const
{
	return mName;
}

QString StringListProperty::getUid() const
{
	return mUid;
}

QString StringListProperty::getHelp() const
{
	return mHelp;
}

void StringListProperty::setHelp(QString val)
{
	if (val == mHelp)
		return;

	mHelp = val;
	emit changed();
}

QVariant StringListProperty::getValueAsVariant() const
{
	return this->getValue();
}

void StringListProperty::setValueFromVariant(QVariant val)
{
	this->setValue(val.value<QStringList>());
}


QStringList StringListProperty::getValue() const
{
	return mValue;
}

bool StringListProperty::setValue(const QStringList& val)
{
	if (val == mValue)
		return false;

	mValue.clear();
	for (int i=0; i<val.size(); ++i)
		if (mRange.contains(val[i]))
			mValue.push_back(val[i]);

	mStore.writeVariant(mValue);
	emit changed();
	return true;
}

QStringList StringListProperty::getValueRange() const
{
	return mRange;
}

void StringListProperty::setValueRange(QStringList range)
{
	mRange = range;
	emit changed();
}

/**If a mapping from internal name to display name has been set, use it.
 * Otherwise return the input.
 */
QString StringListProperty::convertInternal2Display(QString internal)
{
	if (mDisplayNames.count(internal))
		return mDisplayNames[internal];
	return internal;
}

void StringListProperty::setDisplayNames(std::map<QString, QString> names)
{
	mDisplayNames = names;
	emit changed();
}

} // namespace cx

