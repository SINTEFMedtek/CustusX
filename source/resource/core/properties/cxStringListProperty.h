/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTRINGLISTPROPERTY_H
#define CXSTRINGLISTPROPERTY_H

#include "cxResourceExport.h"

#include <QDomElement>
#include <QStringList>
#include "cxStringPropertyBase.h"
#include "cxXmlOptionItem.h"

namespace cx
{

typedef boost::shared_ptr<class StringListProperty> StringListPropertyPtr;

/**
 * \brief Represents one collection of strings.
 *
 * The collection can be selected from a range.
 * The data can be stored within a xml document.
 *
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT StringListProperty: public Property
{
Q_OBJECT
public:
	static StringListPropertyPtr initialize(const QString& uid, QString name, QString help, QStringList value, QStringList range, QDomNode root = QDomNode());

public:
	// inherited interface
	virtual QString getDisplayName() const;///< name of data entity. Used for display to user.
	virtual QString getUid() const;
	virtual QVariant getValueAsVariant() const;
	virtual void setValueFromVariant(QVariant val);

	virtual bool setValue(const QStringList& value); ///< set the data value.
	virtual QStringList getValue() const; ///< get the data value.
	virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual void setHelp(QString val);
	virtual QStringList getValueRange() const; /// range that values can be selected from
	virtual void setValueRange(QStringList range);
	virtual QString convertInternal2Display(QString internal); ///< conversion from internal value to display value
	virtual void setDisplayNames(std::map<QString, QString> names);
	virtual std::map<QString, QString> getDisplayNames() const { return mDisplayNames; }

	virtual void setDisplayName(QString val);

private:
	StringListProperty();
	QString mName;
	QString mUid;
	QString mHelp;
	QStringList mValue;
	QStringList mRange;
	XmlOptionItem mStore;
	std::map<QString, QString> mDisplayNames;
};

} // namespace cx

#endif // CXSTRINGLISTPROPERTY_H
