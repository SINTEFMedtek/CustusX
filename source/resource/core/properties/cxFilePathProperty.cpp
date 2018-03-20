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
#include <QDir>
#include "cxTypeConversions.h"
#include "cxLogger.h"

namespace cx
{


void EmbeddedFilepath::appendRootPath(QString path)
{
    mRoots << path;
}

void EmbeddedFilepath::setFilepath(QString filename)
{
    mFilePath = filename;
}

void EmbeddedFilepath::evaluate(QString* foundRoot, bool* found, QString* foundRelative, QString* foundAbsolute) const
{
    *foundRelative = mFilePath;
    if (!mRoots.empty())
        *foundRoot = mRoots.front();
    *foundAbsolute = mFilePath;
    *found = false;

    foreach (QString root, mRoots)
    {
        root = QDir::cleanPath(root);
        if (!mFilePath.isEmpty() && QDir(root).exists(mFilePath))
        {
            *foundRelative = QDir(root).relativeFilePath(mFilePath);
            *foundRoot = root;
            *foundAbsolute = QDir(root).absoluteFilePath(mFilePath);
            *found = true;

            if (foundRelative->contains(".."))
            {
                // dont use relative paths outside of the roots
                *foundRelative = *foundAbsolute;
            }
            else
            {
                // if the current hit is inside the root, accept immediately
                return;
            }
        }
    }

}

QString EmbeddedFilepath::getRelativeFilepath() const
{
    bool found = false;
    QString root, relative, absolute;
    this->evaluate(&root, &found, &relative, &absolute);

    return relative;
}

QString EmbeddedFilepath::getAbsoluteFilepath() const
{
    bool found = false;
    QString root, relative, absolute;
    this->evaluate(&root, &found, &relative, &absolute);

    return absolute;
}

bool EmbeddedFilepath::exists() const
{
    bool found = false;
    QString root, relative, absolute;
    this->evaluate(&root, &found, &relative, &absolute);

    return found;
}

QString EmbeddedFilepath::getRootPath() const
{
    bool found = false;
    QString root, relative, absolute;
    this->evaluate(&root, &found, &relative, &absolute);

    return root;
}

QStringList EmbeddedFilepath::getRootPaths() const
{
    return mRoots;
}

} // namespace cx




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

QString FilePathProperty::getDisplayName() const
{
    return mName;
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


QString FilePathProperty::getValue() const
{
    return mFilePath.getRelativeFilepath();
}

bool FilePathProperty::setValue(const QString& val)
{
    if (val == this->getValue())
        return false;

    mFilePath.setFilepath(val);
    mStore.writeValue(mFilePath.getRelativeFilepath());
    emit valueWasSet();
    emit changed();
    return true;
}

EmbeddedFilepath FilePathProperty::getEmbeddedPath()
{
    return mFilePath;
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
