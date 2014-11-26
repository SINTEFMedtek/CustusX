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

#include "cxSpaceDataAdapterXml.h"
#include <iostream>
#include <QDomElement>
#include <QStringList>
#include "cxTypeConversions.h"

namespace cx
{

SpaceDataAdapterXml::SpaceDataAdapterXml() : 	mIsReadOnly(false)
{

}

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
SpaceDataAdapterXmlPtr SpaceDataAdapterXml::initialize(const QString& uid, QString name, QString help, Space value, std::vector<Space> range, QDomNode root)
{
	SpaceDataAdapterXmlPtr retval(new SpaceDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = Space::fromString(retval->mStore.readValue(value.toString()));
	retval->mAllowOnlyValuesInRange = true;
	return retval;
}

void SpaceDataAdapterXml::setReadOnly(bool val)
{
	mIsReadOnly = val;
	emit changed();
}

QString SpaceDataAdapterXml::getDisplayName() const
{
	return mName;
}

QString SpaceDataAdapterXml::getUid() const
{
	return mUid;
}

QString SpaceDataAdapterXml::getHelp() const
{
	return mHelp;
}

void SpaceDataAdapterXml::setHelp(QString val)
{
	if (val == mHelp)
		return;

	mHelp = val;
	emit changed();
}


Space SpaceDataAdapterXml::getValue() const
{
	return mValue;
}

bool SpaceDataAdapterXml::setValue(const Space& val)
{
	if (val == mValue)
		return false;

	mValue = val;
	mStore.writeValue(val.toString());
	emit valueWasSet();
	emit changed();
	return true;
}

std::vector<Space> SpaceDataAdapterXml::getValueRange() const
{
	return mRange;
}

void SpaceDataAdapterXml::setValueRange(std::vector<Space> range)
{
	mRange = range;
	emit changed();
}

/**If a mapping from internal name to display name has been set, use it.
 * Otherwise return the input.
 */
QString SpaceDataAdapterXml::convertInternal2Display(Space internal)
{
	return internal.toString();
//	if (mDisplayNames.count(internal))
//		return mDisplayNames[internal];
//	return internal.toString();
}

//void SpaceDataAdapterXml::setDisplayNames(std::map<Space, QString> names)
//{
//	mDisplayNames = names;
//	emit changed();
//}

} // namespace cx
