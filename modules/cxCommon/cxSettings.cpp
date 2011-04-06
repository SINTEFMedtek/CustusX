#include "cxSettings.h"

namespace cx
{

Settings *Settings::mInstance = NULL;
Settings* settings() { return Settings::getInstance(); }
Settings* Settings::getInstance()
{
  if(mInstance == NULL)
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
{}

Settings::~Settings()
{}

void Settings::initialize()
{
  //TODO init QSettings
}

void Settings::setValue(const QString& key, const QVariant& value)
{}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const
{}

void Settings::sync()
{}
}
