/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilePreviewProperty.h"

namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
FilePreviewPropertyPtr FilePreviewProperty::initialize(const QString &uid, QString name, QString help, QString value, QStringList paths, QDomNode root)
{
	FilePreviewPropertyPtr retval(new FilePreviewProperty());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mFilePath.setFilepath(value);
	foreach (QString path, paths)
		retval->mFilePath.appendRootPath(path);
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mFilePath.setFilepath(retval->mStore.readValue(value));
	return retval;
}

FilePreviewProperty::FilePreviewProperty()
{
}

}//cx
