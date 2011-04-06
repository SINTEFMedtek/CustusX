#ifndef CXSETTINGS_H_
#define CXSETTINGS_H_

#include <QObject>
#include <QSettings>

namespace cx
{

/**
 * Settings
 *
 * \brief Customized interface for setting values in QSettings
 *
 * \date Apr 6, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class Settings : public QObject
{
  Q_OBJECT

public:
  static Settings* getInstance(); ///< returns the only instance of this class
  static void destroyInstance();     ///< destroys the only instance of this class

  void setValue(const QString& key, const QVariant& value);
  QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
  void sync();

private:
  Settings();
  virtual ~Settings();

  void initialize();

  QSettings mSetting;

  static Settings* mInstance; ///< The only instance of this class that can exist.
};

/**Shortcut for accessing the settings instance.*/
Settings* settings();
}

#endif /* CXSETTINGS_H_ */
