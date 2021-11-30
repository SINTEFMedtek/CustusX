/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilePathProperty.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>
#include "cxTypeConversions.h"
#include "cxLogger.h"

namespace cx
{

FilePathProperty::FilePathProperty()
{

}

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
FilePathPropertyPtr FilePathProperty::initialize(const QString& uid, QString name, QString help, QString value, QStringList paths, QDomNode root)
{
	FilePathPropertyPtr retval(new FilePathProperty());
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

QString FilePathProperty::getUid() const
{
	return mUid;
}

QString FilePathProperty::getHelp() const
{
	return mHelp;
}

void FilePathProperty::setHelp(QString val)
{
	if (val == mHelp)
		return;

	mHelp = val;
	emit changed();
}

QVariant FilePathProperty::getValueAsVariant() const
{
	return this->getValue();
}

void FilePathProperty::setValueFromVariant(QVariant val)
{
	this->setValue(val.toString());
}

} // namespace cx
