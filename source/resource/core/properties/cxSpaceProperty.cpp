/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSpaceProperty.h"
#include <iostream>
#include <QDomElement>
#include <QStringList>
#include "cxTypeConversions.h"
#include "cxSpaceProvider.h"

namespace cx
{

SpaceProperty::SpaceProperty() : 	mIsReadOnly(false)
{

}

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
SpacePropertyPtr SpaceProperty::initialize(const QString& uid, QString name, QString help, Space value, std::vector<Space> range, QDomNode root)
{
	SpacePropertyPtr retval(new SpaceProperty());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = Space::fromString(retval->mStore.readValue(value.toString()));
	retval->mAllowOnlyValuesInRange = true;
	return retval;
}

void SpaceProperty::setSpaceProvider(SpaceProviderPtr provider)
{
	mProvider = provider;
	connect(mProvider.get(), &SpaceProvider::spaceAddedOrRemoved, this, &SpaceProperty::providerChangedSlot);
	this->providerChangedSlot();
}

void SpaceProperty::providerChangedSlot()
{
	std::map<QString, QString> names = mProvider->getDisplayNamesForCoordRefObjects();
	this->setRefObjectDisplayNames(names);

	std::vector<CoordinateSystem> spaces = mProvider->getSpacesToPresentInGUI();
	this->setValueRange(spaces);
}


void SpaceProperty::setReadOnly(bool val)
{
	mIsReadOnly = val;
	emit changed();
}

QString SpaceProperty::getDisplayName() const
{
	return mName;
}

QString SpaceProperty::getUid() const
{
	return mUid;
}

QString SpaceProperty::getHelp() const
{
	return mHelp;
}

void SpaceProperty::setHelp(QString val)
{
	if (val == mHelp)
		return;

	mHelp = val;
	emit changed();
}


Space SpaceProperty::getValue() const
{
	return mValue;
}

bool SpaceProperty::setValue(const Space& val)
{
	if (val == mValue)
		return false;

	mValue = val;
	mStore.writeValue(val.toString());
	emit valueWasSet();
	emit changed();
	return true;
}

std::vector<Space> SpaceProperty::getValueRange() const
{
	return mRange;
}

void SpaceProperty::setValueRange(std::vector<Space> range)
{
	mRange = range;
	emit changed();
}

/**If a mapping from internal name to display name has been set, use it.
 * Otherwise return the input.
 */
QString SpaceProperty::convertRefObjectInternal2Display(QString internal)
{
	if (mDisplayNames.count(internal))
		return mDisplayNames[internal];
	return internal;
}

void SpaceProperty::setRefObjectDisplayNames(std::map<QString, QString> names)
{
	mDisplayNames = names;
	emit changed();
}

} // namespace cx
