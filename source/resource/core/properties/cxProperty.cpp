/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxProperty.h"

namespace cx
{

Property::Property() :
		QObject(), mEnabled(true), mAdvanced(false), mGroup("")
{}


PropertyPtr Property::findProperty(std::vector<PropertyPtr> properties, QString id)
{
	for(int i=0; i<properties.size(); ++i)
	{
		PropertyPtr adapter = properties[i];
		if(QString::compare(adapter->getUid(), id) == 0)
		{
			return adapter;
		}
	}
	return PropertyPtr();
}

bool Property::getEnabled() const
{
	return mEnabled;
}

bool Property::getAdvanced() const
{
	return mAdvanced;
}

QString Property::getGroup() const
{
	return mGroup;
}

bool Property::setEnabled(bool enabling)
{

	if(mEnabled == enabling)
		return false;

	mEnabled = enabling;
	emit changed();

	return true;
}

bool Property::setAdvanced(bool advanced)
{
	if(advanced == mAdvanced)
		return false;

	mAdvanced = advanced;
	emit changed();

	return true;
}

bool Property::setGroup(QString name)
{
	if(name == mGroup)
		return false;

	mGroup = name;
	emit changed();

	return true;
}


} //namespace cx
