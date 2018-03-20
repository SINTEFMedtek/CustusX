/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFILEHELPERS_H
#define CXFILEHELPERS_H

#include "cxResourceExport.h"

#include <QString>
#include <QDir>

namespace cx
{

/**
* \file
* \addtogroup cx_resource_core_utilities
* @{
*/

/** Remove a nonempty folder recursively
 *
 * http://stackoverflow.com/questions/11050977/removing-a-non-empty-folder-in-qt
 *
 * \ingroup cxResourceUtilities
 * \date jan 26, 2013
 * \author christiana
 */
cxResource_EXPORT bool removeNonemptyDirRecursively(const QString& dirName);

//From http://stackoverflow.com/questions/2536524/copy-directory-using-qt
cxResource_EXPORT bool copyRecursively(QString sourceDir, QString destinationDir, bool overWriteDirectory);

cxResource_EXPORT QFileInfoList getDirs(QString path);
cxResource_EXPORT QStringList getAbsolutePathToFiles(QString path, QStringList nameFilters, bool includeSubDirs = false);
cxResource_EXPORT QStringList getAbsolutePathToXmlFiles(QString path, bool includeSubDirs = false);

/** Open file, check if it ends with a newline, write newline if it doesn´t
 */
cxResource_EXPORT void forceNewlineBeforeEof(QString path);

/**
 * @\}
 */

} // namespace cx


#endif // CXFILEHELPERS_H
