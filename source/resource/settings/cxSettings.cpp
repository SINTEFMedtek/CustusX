// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#include "cxSettings.h"
#include "cxDataLocations.h"

namespace cx
{

Settings *Settings::mInstance = NULL;
Settings* settings()
{
	return Settings::getInstance();
}
Settings* Settings::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new Settings();
		mInstance->initialize();
	}
	return mInstance;
}

void Settings::destroyInstance()
{
	delete mInstance;
	mInstance = NULL;
}

Settings::Settings()
{
}

Settings::~Settings()
{
}

void Settings::initialize()
{
	QString filename = cx::DataLocations::getSettingsPath() + "/settings.ini";
	//QString filename = cx::DataLocations::getRootConfigPath() + "/settings/settings.ini";
	mSettings.reset(new QSettings(filename, QSettings::IniFormat));
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
