/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



/*
 * sscBoolProperty.h
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#ifndef CXBOOLPROPERTY_H_
#define CXBOOLPROPERTY_H_

#include "cxResourceExport.h"

#include <QDomElement>

#include "cxBoolPropertyBase.h"
#include "cxXmlOptionItem.h"

namespace cx
{

typedef boost::shared_ptr<class BoolProperty> BoolPropertyPtr;

/** Property for boolean values.
 *
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT BoolProperty: public BoolPropertyBase
{
Q_OBJECT
public:
	virtual ~BoolProperty()
	{
	}

	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static BoolPropertyPtr initialize(const QString& uid, QString name, QString help, bool value, QDomNode root =
		QDomNode());

public:
	// basic methods
	virtual QString getDisplayName() const; ///< name of data entity. Used for display to user.
	virtual QString getUid() const;
	virtual bool setValue(bool value); ///< set the data value.
	virtual bool getValue() const; ///< get the data value.

public:
	// optional methods
	virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
    void setHelp(QString val);

signals:
	void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
	BoolProperty() {}
	QString mName;
	QString mUid;
	QString mHelp;
	bool mValue;
	XmlOptionItem mStore;

};

}

#endif /* CXBOOLPROPERTY_H_ */
