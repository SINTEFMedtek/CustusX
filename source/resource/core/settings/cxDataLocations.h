/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	static void deletePersistentWritablePath(); ///< Deletes the folder called *_settings
	static QString getCachePath(); ///< return path to a folder that is used during execution, will be cleared at start and stop.
	static QStringList getDefaultPluginsPath(); ///< return the folder where plugins should be located, by default.

	static void setTestMode(); ///< set a testing mode that changes location of settings files to a temp folder.
	static QString findExecutableInStandardLocations(QString filename); ///< look for an exe in the same folder as the executable or bundle.

	static QStringList appendStringToAllElements(QStringList root, QString suffix);

	static QString findConfigFolder(QString pathRelativeToConfigRoot, QString alternativeAbsolutePath = "");
	static QString findConfigFilePath(QString fileName, QString pathRelativeToConfigRoot, QString alternativeAbsolutePath = "");
	static QString findConfigPath(QString fileName, QString pathRelativeToConfigRoot, QString alternativeAbsolutePath = "");

	static void setWebsiteURL(QString websiteUrl);
	static QString getWebsiteURL();
	static QString getUploadsUrl();
	static QString getWebsiteUserDocumentationURL();

	static bool isRunFromBuildFolder();
	static bool isTestMode();

private:
	static bool mTestMode;
	static bool mRunFromBuildFolder;
	static bool mBuildFolderChecked;
	static QString mWebsiteUrl;
	static QString readTestDataPathFromFile(QString filename);
	static QString checkExecutableExist(QString path, QString filename);

};

} // namespace cx

#endif /* CXDATALOCATIONS_H_ */
