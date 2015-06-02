/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
