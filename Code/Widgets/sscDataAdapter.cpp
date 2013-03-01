// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscDataAdapter.h"

/*
 * cxDataAdapter.cpp
 *
 * \date Dec 21, 2012
 * \author Janne Beate Bakeng, SINTEF
 */

DataAdapter::DataAdapter() :
		QObject(), mEnabled(true), mAdvanced(false), mGroup("")
{}

bool DataAdapter::getEnabled() const
{
	return mEnabled;
}

bool DataAdapter::getAdvanced() const
{
	return mAdvanced;
}

QString DataAdapter::getGroup() const
{
	return mGroup;
}

bool DataAdapter::setEnabled(bool enabling)
{

	if(mEnabled == enabling)
		return false;

	mEnabled = enabling;
	emit changed();

	return true;
}

bool DataAdapter::setAdvanced(bool advanced)
{
	if(advanced == mAdvanced)
		return false;

	mAdvanced = advanced;
	emit changed();

	return true;
}

bool DataAdapter::setGroup(QString name)
{
	if(name == mGroup)
		return false;

	mGroup = name;
	emit changed();

	return true;
}


