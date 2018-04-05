/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXBOOLPROPERTYBASE_H_
#define CXBOOLPROPERTYBASE_H_

#include "cxResourceExport.h"

#include "cxProperty.h"

namespace cx
{

/** Property interface for boolean values.
 *
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT BoolPropertyBase: public Property
{
Q_OBJECT
public:
	virtual ~BoolPropertyBase()
	{
	}

public:
	// basic methods
	virtual QString getDisplayName() const = 0; ///< name of data entity. Used for display to user.
	virtual bool setValue(bool value) = 0; ///< set the data value.
	virtual bool getValue() const = 0; ///< get the data value.

	virtual QVariant getValueAsVariant() const
	{
		return QVariant(this->getValue());
	}
	virtual void setValueFromVariant(QVariant val)
	{
		this->setValue(val.toBool());
	}

public:
	// optional methods
	virtual QString getHelp() const
	{
		return QString();
	} ///< return a descriptive help string for the data, used for example as a tool tip.
	//virtual void connectValueSignals(bool on) {} ///< set object to emit changed() when applicable

	//signals:
	//  void changed(); ///< emit when the underlying data value is changed: The user interface will be updated.
};
typedef boost::shared_ptr<BoolPropertyBase> BoolPropertyBasePtr;

}

#endif /* CXBOOLPROPERTYBASE_H_ */
