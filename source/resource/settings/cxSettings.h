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
#ifndef CXSETTINGS_H_
#define CXSETTINGS_H_

#include "boost/shared_ptr.hpp"
#include <QObject>
#include <QSettings>

namespace cx
{

/**
* \file
* \addtogroup cxResourceSettings
* @{
*/

typedef boost::shared_ptr<class QSettings> QSettingsPtr;

/**
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

  //forwarding functions from QSettings
  void setValue(const QString& key, const QVariant& value);
  QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

  bool contains(const QString& key) const;
  QString fileName() const;

  void sync();

  //additional functionality

signals:
  void valueChangedFor(QString key);

private:
  Settings();
  virtual ~Settings();

  void initialize();

  QSettingsPtr mSettings;

  static Settings* mInstance; ///< The only instance of this class that can exist.
};

/**\brief Shortcut for accessing the settings instance.*/
Settings* settings();

/**
* @}
*/

}

#endif /* CXSETTINGS_H_ */
