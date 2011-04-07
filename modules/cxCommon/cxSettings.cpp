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
  mSettings.reset(new QSettings());
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
