/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxSettings.h"
#include "cxTypeConversions.h"
#include <iostream>
//#include "cxDataLocations.h"
#include "cxProfile.h"

namespace cx
{

//Settings *Settings::mInstance = NULL;
Settings* settings()
{
	return profile()->getSettings();
}

Settings::Settings()
{
}

Settings::~Settings()
{
}

void Settings::resetFile(QString filename)
{
	QStringList keys;
	if (mSettings)
		keys << mSettings->allKeys();

	mSettings.reset(new QSettings(filename, QSettings::IniFormat));

	if (mSettings)
		keys << mSettings->allKeys();

	keys.removeDuplicates();

	foreach (QString key, keys)
		emit valueChangedFor(key);
}

void Settings::setValueIfNotDefault(const QString& key, const QVariant& value, const QVariant& defaultValue)
{
	if (value==defaultValue)
		return;
	this->setValue(key, value);
}

void Settings::setValue(const QString& key, const QVariant& value)
{
	mSettings->setValue(key, value);
	this->sync();

	emit valueChangedFor(key);
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const
{
	return mSettings->value(key, defaultValue);
}

bool Settings::contains(const QString& key) const
{
	return mSettings->contains(key);
}

QString Settings::fileName() const
{
	return mSettings->fileName();
}

void Settings::sync()
{
	mSettings->sync();
}
}
