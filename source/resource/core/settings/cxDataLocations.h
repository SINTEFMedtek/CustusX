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
#include <QObject>
#include "boost/shared_ptr.hpp"

class QSettings;

namespace cx
{

/** Helper functions for accessing file paths and similar stuff.
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
	static QString getPersistentWritablePath(); ///< Path to location usable for persistent and temporary storage of config. Do not use directly, prefer to create methods displaying subpaths.
	static QString getCachePath(); ///< return path to a folder that is used during execution, will be cleared at start and stop.
	static QStringList getDefaultPluginsPath(); ///< return the folder where plugins should be located, by default.

	static void setTestMode(); ///< set a testing mode that changes location of settings files to a temp folder.
	static QString findExecutableInStandardLocations(QString filename); ///< look for an exe in the same folder as the executable or bundle.

	static QStringList appendStringToAllElements(QStringList root, QString suffix);

	static QString findConfigFolder(QString pathRelativeToConfigRoot, QString alternativeAbsolutePath = "");
	static QString findConfigFilePath(QString fileName, QString pathRelativeToConfigRoot, QString alternativeAbsolutePath = "");
	static QString findConfigPath(QString fileName, QString pathRelativeToConfigRoot, QString alternativeAbsolutePath = "");

	static QString getWebsiteURL();
	static QString getWebsiteUserDocumentationURL();

	static bool isRunFromBuildFolder();
private:
	static bool mTestMode;
	static bool mRunFromBuildFolder;
	static bool mBuildFolderChecked;
	static QString readTestDataPathFromFile(QString filename);
	static QString checkExecutableExist(QString path, QString filename);

};

} // namespace cx

#endif /* CXDATALOCATIONS_H_ */
