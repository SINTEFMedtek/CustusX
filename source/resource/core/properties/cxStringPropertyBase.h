/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXSTRINGPROPERTYBASE_H_
#define CXSTRINGPROPERTYBASE_H_

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QComboBox>
#include "cxProperty.h"

namespace cx
{

/**\brief Abstract interface for interaction with internal string-valued data
 *
 * The class provides a bridge between general user interface code and specific
 * data structures. An implementation connects to a single data value.
 *
 *
 * Minimal implementation:
  \verbatim
       virtual QString getDisplayName() const;
       virtual bool setValue(QString value);
       virtual QString getValue() const;
       void changed();
  \endverbatim
 *
 * By implementing these methods you can set and get values, and the data has a name.
 * The changed() signal is used to make sure the user interface is updated even when
 * data is changed by some other source.
 *
 *
 * For more control use the methods:
  \verbatim
       virtual QStringList getValueRange() const;
  \endverbatim
 *
 *
 * If there is a difference between the internal data representation and
 * how you want to present them, use:
 *
  \verbatim
       virtual QString convertInternal2Display(QString internal);
  \endverbatim
 *
 * When testing, or during development, you can use the StringPropertyNull
 * as a dummy implementation.
 *
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT StringPropertyBase: public Property
{
	Q_OBJECT
public:
	StringPropertyBase() : mGuiRepresentation(grSTRING) {}
	virtual ~StringPropertyBase(){}

public:
	enum GuiRepresentation
	{
		grSTRING,
		grFILENAME
	};

	// basic methods
	virtual QString getDisplayName() const = 0;///< name of data entity. Used for display to user.

	virtual QVariant getValueAsVariant() const
	{
		return QVariant(this->getValue());
	}
	virtual void setValueFromVariant(QVariant val)
	{
		this->setValue(val.toString());
	}

	virtual QString getUid() const { return this->getDisplayName()+"_uid"; }
	virtual bool setValue(const QString& value) = 0; ///< set the data value.
	virtual QString getValue() const = 0; ///< get the data value.

public:
	/*
	 * If set: setValue will always fail. Adapter is used for display only.
	 */
	virtual bool isReadOnly() const { return false; }
	/*
	 * If true: value will always be within range. setValue using other values will fail.
	 */
	virtual bool getAllowOnlyValuesInRange() const { return true; }
	// optional methods
	virtual QString getHelp() const
	{
		return QString();
	} ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual QStringList getValueRange() const
	{
		return QStringList();
	} /// range of value. Use if data is constrained to a set.
	virtual QString convertInternal2Display(QString internal)
	{
		return internal;
	} ///< conversion from internal value to display value

	virtual void setGuiRepresentation(GuiRepresentation type) { mGuiRepresentation = type; };
	virtual GuiRepresentation getGuiRepresentation() { return mGuiRepresentation; };

protected:
	GuiRepresentation mGuiRepresentation;

};
typedef boost::shared_ptr<StringPropertyBase> StringPropertyBasePtr;

} // namespace cx

#endif /* CXSTRINGPROPERTYBASE_H_ */
