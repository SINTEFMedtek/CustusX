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
#ifndef CXDATALOCATIONS_H_
#define CXDATALOCATIONS_H_

#include "cxResourceExport.h"

#include <QString>

namespace cx
{

/**Helper functions for accessing file paths and similar stuff.
 * \ingroup cx_resource_core_settings
 *  \date Jun 22, 2010
 *  \author christiana
 *
 */
class cxResource_EXPORT DataLocations
{
public:
  static QString getBundlePath(); ///< return the folder where the bundle or executable are located.
	static QString getRootConfigPath(); ///< return path to root config folder. May be replaced with getExistingConfigPath()
  static QStringList getRootConfigPaths();

  static QString getTestDataPath(); ///< return path to test data folder
  static QString getLargeTestDataPath(); ///< return path to test data folder containing large data sets
  static QString getExistingTestData(QString pathRelativeToTestDataRoot, QString filename=""); ///< Return full path to test data, both normal and large repositories are searched.

  static QString getDocPath(); ///< return path to folder containing documentation files
  static QStringList getToolsPaths(); ///< return path to folder containing all defined tools
  static QStringList getApplicationToolConfigPaths(); ///< return path to tool config folder, based on choosen application
  static QString getToolConfigFilePath(); ///< return users path to tool config file
  static QString getAudioConfigFilePath(); ///< return users path to audio config file
  static QString getShaderPath(); ///< return the path to installed shaders. empty string if not installed.
  static QString getSettingsPath(); ///< return path where settings are to be stored.
  static QString getXmlSettingsFile(); ///< return a filename for a xml file containing global custusX data (equal to settings above)
	static QString getCachePath(); ///< return path to a folder that is used during execution, will be cleared at start and stop.
	static QStringList getDefaultPluginsPath(); ///< return the folder where plugins should be located, by default.

	static QString getExistingConfigPath(QString pathRelativeToConfigRoot, QString alternativeAbsolutePath, QString filename=""); ///< Return folder path. Path is typically either relative to config root (created during installation), or an alternative absolute path (typically in the source tree)

  static void setTestMode(); ///< set a testing mode that changes location of settings files to a temp folder.

private:
  static bool mTestMode;
  static QString readTestDataPathFromFile(QString filename);
  static QStringList appendStringToAllElements(QStringList root, QString suffix);
//  static DataLocations* mInstance;
//  static DataLocations getInstance();

};

} // namespace cx

#endif /* CXDATALOCATIONS_H_ */
