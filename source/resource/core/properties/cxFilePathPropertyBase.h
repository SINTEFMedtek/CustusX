/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFILEPATHPROPERTYBASE_H
#define CXFILEPATHPROPERTYBASE_H

#include "cxProperty.h"
#include <QObject>
#include <QDomElement>
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

/**
 * \class FilePathPropertyBase
 *
 * \brief Base class for properties using a file path.
 *
 * \date Apr 23, 2020
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT FilePathPropertyBase: public Property
{
	Q_OBJECT
public:
	FilePathPropertyBase();
	virtual ~FilePathPropertyBase(){}

	// basic methods
	virtual QString getDisplayName() const;// = 0;///< name of data entity. Used for display to user.

	virtual QVariant getValueAsVariant() const
	{
		return QVariant(this->getValue());
	}
	virtual void setValueFromVariant(QVariant val)
	{
		this->setValue(val.toString());
	}

	virtual QString getUid() const { return this->getDisplayName()+"_uid"; }
	virtual bool setValue(const QString& value);// = 0; ///< set the data value.
	virtual QString getValue() const;// = 0; ///< get the data value.

	EmbeddedFilepath getEmbeddedPath();

signals:
	void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

protected:
	QString mName;
	QString mUid;
	QString mHelp;
	EmbeddedFilepath mFilePath;
	XmlOptionItem mStore;

};
}//cx
#endif // CXFILEPATHPROPERTYBASE_H
