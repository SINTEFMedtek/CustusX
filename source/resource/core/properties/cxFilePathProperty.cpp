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
