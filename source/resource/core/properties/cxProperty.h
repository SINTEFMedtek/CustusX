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

#ifndef CXPROPERTY_H_
#define CXPROPERTY_H_

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <QString>
#include <QObject>
#include <QVariant>

namespace cx {
typedef boost::shared_ptr<class Property> PropertyPtr;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class ViewService> ViewServicePtr;

/**\brief Superclass for all data adapters.
 *
 * The Property are an abstraction mechanism that separates
 * data values from the user interface. Properties for strings,
 * doubles, booleans and so on publish their value in a generic
 * manner, thus enabling us to write generic widgets for displaying
 * and changing them.
 *
 * \ingroup cx_resource_core_properties
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 * \date Jun 27, 2010
 *
 */

class cxResource_EXPORT Property: public QObject
{
	Q_OBJECT

public:
	Property();
	virtual ~Property(){}

	static PropertyPtr findProperty(std::vector<PropertyPtr> properties, QString id);

public:
	virtual QString getDisplayName() const = 0; ///< name of data entity. Used for display to user.
	virtual QString getUid() const = 0;

	virtual QVariant getValueAsVariant() const = 0;
	virtual void setValueFromVariant(QVariant val) = 0;

	virtual bool getEnabled() const; ///< Get the enabled/disabled state of the Property.
    virtual bool getAdvanced() const; ///< Get the advanced flag of the adapter
	virtual QString getGroup() const; ///< Flag the adapter as part of a group

public slots:
	virtual bool setEnabled(bool enabled); ///< Set the enabled/disabled state of the Property.
	virtual bool setAdvanced(bool advanced); ///< Set the advanced flag of the adapter
	virtual bool setGroup(QString name); ///< Flag the adapter as part of a group

signals:
	void changed(); ///< emit when the underlying data value is changed: The user interface will be updated.

protected:
	bool mEnabled; //< a property can be in either a enabled or disabled state
	bool mAdvanced; //< flag marking this property value as a advanced option
	QString mGroup; //< tag marking this property as part of a group with this name
};
} //namespace cx

#endif /* CXPROPERTY_H_ */
