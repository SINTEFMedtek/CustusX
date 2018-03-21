/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
