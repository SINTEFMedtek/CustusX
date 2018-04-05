/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSETTINGS_H_
#define CXSETTINGS_H_

#include "cxResourceExport.h"

#include "boost/shared_ptr.hpp"
#include <QObject>
#include <QSettings>

namespace cx
{

/**
* \file
* \addtogroup cx_resource_core_settings
* @{
*/

typedef boost::shared_ptr<class QSettings> QSettingsPtr;

/**
 * \brief Customized interface for setting values in QSettings
 *
 * \date Apr 6, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT Settings : public QObject
{
  Q_OBJECT

public:

	void setValueIfNotDefault(const QString& key, const QVariant& value, const QVariant& defaultValue);
  //forwarding functions from QSettings
  void setValue(const QString& key, const QVariant& value);
  QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

  bool contains(const QString& key) const;
  QString fileName() const;

  void sync();

  void resetFile(QString filename); ///< internal use only: reset internal settings file.
  Settings();
  virtual ~Settings();

  /** Initialize a setting if undefined.
   */
  template<class T>
  void fillDefault(QString name, T value)
  {
	  if (!this->contains(name))
		  this->setValue(name, value);
  }

signals:
  void valueChangedFor(QString key);

private slots:
private:
  QSettingsPtr mSettings;

};

/**\brief Shortcut for accessing the settings instance.*/
cxResource_EXPORT Settings* settings();

/**
* @}
*/

}

#endif /* CXSETTINGS_H_ */
