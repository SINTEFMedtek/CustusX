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

#ifndef CXFILEPATHPROPERTY_H
#define CXFILEPATHPROPERTY_H

#include "cxResourceExport.h"

#include <QDomElement>
#include <QStringList>
#include "cxStringPropertyBase.h"
#include "cxXmlOptionItem.h"

namespace cx
{


/** Represent a file that is located in a location relative
 *  to a root folder or one of several root folders.
 *
 *  Examples are
 *   - relative to the executable,
 *   - relative to one of several configuration roots: the factory root and the custom root
 *
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT EmbeddedFilepath
{
public:
    void appendRootPath(QString path);
    void setFilepath(QString filename); ///< relative to one of the root paths or absolute

    QString getRelativeFilepath() const; ///< return filepath relative to root.
    QString getAbsoluteFilepath() const; ///< return absolute filepath, select the existing root
    bool exists() const;
    QString getRootPath() const; ///< return the root of the existing root, first if no existing.
    QStringList getRootPaths() const; ///< return the root of the existing root, first if no existing.

private:
    void evaluate(QString* foundRoot, bool* found, QString* foundRelative, QString* foundAbsolute) const;
    QStringList mRoots;
    QString mFilePath;
};

typedef boost::shared_ptr<class FilePathProperty> FilePathPropertyPtr;

/**
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT FilePathProperty: public Property
{
Q_OBJECT
public:
    /** Make sure one given option exists witin root.
     * If not present, fill inn the input defaults.
     */
    static FilePathPropertyPtr initialize(const QString& uid, QString name, QString help, QString value,
        QStringList paths, QDomNode root = QDomNode());
public:
    // inherited interface
    virtual QString getDisplayName() const;///< name of data entity. Used for display to user.
    virtual QString getUid() const;

    virtual QVariant getValueAsVariant() const;
    virtual void setValueFromVariant(QVariant val);

    virtual bool setValue(const QString& value); ///< set the data value.
    virtual QString getValue() const; ///< get the data value.
    virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
    virtual void setHelp(QString val);

    EmbeddedFilepath getEmbeddedPath();

signals:
    void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
    FilePathProperty();
    QString mName;
    QString mUid;
    QString mHelp;
    EmbeddedFilepath mFilePath;
    XmlOptionItem mStore;
};


} // namespace cx

#endif // CXFILEPATHPROPERTY_H
