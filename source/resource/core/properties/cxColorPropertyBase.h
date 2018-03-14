/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCOLORPROPERTYBASE_H_
#define CXCOLORPROPERTYBASE_H_

#include "cxResourceExport.h"

#include <QColor>
#include "cxProperty.h"


namespace cx
{

/** Property interface for QColor
 *
 *
 * \ingroup cx_resource_core_properties
 * \date Nov 22, 2012
 * \author christiana
 */
class cxResource_EXPORT ColorPropertyBase: public Property
{
Q_OBJECT
public:
	virtual ~ColorPropertyBase() {}

public:
    // basic methods
    virtual QString getDisplayName() const = 0; ///< name of data entity. Used for display to user.
    virtual bool setValue(QColor value) = 0; ///< set the data value.
    virtual QColor getValue() const = 0; ///< get the data value.

	virtual QVariant getValueAsVariant() const
	{
		return QVariant(this->getValue());
	}

	virtual void setValueFromVariant(QVariant value)
	{
		this->setValue(value.value<QColor>());
	}

public:
    // optional methods
    virtual QString getHelp() const
    {
        return QString();
    } ///< return a descriptive help string for the data, used for example as a tool tip.
};
typedef boost::shared_ptr<ColorPropertyBase> ColorPropertyBasePtr;

} // namespace cx

#endif // CXCOLORPROPERTYBASE_H_
