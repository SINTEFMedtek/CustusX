/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
