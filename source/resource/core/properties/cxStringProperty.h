/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTRINGPROPERTY_H_
#define CXSTRINGPROPERTY_H_

#include "cxResourceExport.h"

#include <QDomElement>
#include <QStringList>
#include "cxStringPropertyBase.h"
#include "cxXmlOptionItem.h"

namespace cx
{

typedef boost::shared_ptr<class StringProperty> StringPropertyPtr;

/**
 * \brief Represents one option of the string type.
 *  The data are stored within a xml document.
 *
 *  The option node has this layout:
   \verbatim
    <option id="Processor" value="3.14"/>
   \endverbatim
 *
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT StringProperty: public StringPropertyBase
{
Q_OBJECT
public:
	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
    static StringPropertyPtr initialize(const QString& uid, QString name, QString help, QString value, QStringList range, QDomNode root = QDomNode());
	static StringPropertyPtr initialize(const QString& uid, QString name, QString help, QString value, QDomNode root = QDomNode());

public:
	// inherited interface
	virtual QString getDisplayName() const;///< name of data entity. Used for display to user.
	virtual QString getUid() const;
	virtual bool setValue(const QString& value); ///< set the data value.
	virtual QString getValue() const; ///< get the data value.
	virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
    virtual void setHelp(QString val);
	virtual QStringList getValueRange() const; /// range of value. Use if data is constrained to a set.
	virtual void setValueRange(QStringList range);
	virtual QString convertInternal2Display(QString internal); ///< conversion from internal value to display value
	virtual void setDisplayNames(std::map<QString, QString> names);
	virtual std::map<QString, QString> getDisplayNames() const { return mDisplayNames; }

	virtual bool isReadOnly() const { return mIsReadOnly; }
	virtual bool getAllowOnlyValuesInRange() const { return mAllowOnlyValuesInRange; }
	void setReadOnly(bool val);
	virtual void setDisplayName(QString val);


signals:
	void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
	StringProperty();
	QString mName;
	QString mUid;
	QString mHelp;
	QString mValue;
	QStringList mRange;
	XmlOptionItem mStore;
	std::map<QString, QString> mDisplayNames;
	bool mIsReadOnly;
	bool mAllowOnlyValuesInRange;
};


} // namespace cx

#endif /* CXSTRINGPROPERTY_H_ */
