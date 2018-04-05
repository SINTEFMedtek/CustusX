/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSPACEPROPERTYBASE_H_
#define CXSPACEPROPERTYBASE_H_

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QComboBox>
#include "cxProperty.h"
#include "cxCoordinateSystemHelpers.h"

namespace cx
{

/**
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT SpacePropertyBase: public Property
{
	Q_OBJECT
public:
	SpacePropertyBase() {}
	virtual ~SpacePropertyBase(){}

public:
	// basic methods
	virtual QString getDisplayName() const = 0;///< name of data entity. Used for display to user.

	virtual QVariant getValueAsVariant() const
	{
		return QVariant(this->getValue().toString());
	}
	virtual void setValueFromVariant(QVariant val)
	{
		this->setValue(Space::fromString(val.toString()));
	}

	virtual QString getUid() const { return this->getDisplayName()+"_uid"; }
	virtual bool setValue(const Space& value) = 0; ///< set the data value.
	virtual Space getValue() const = 0; ///< get the data value.

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
	virtual std::vector<Space> getValueRange() const
	{
		return std::vector<Space>();
	} /// range of value. Use if data is constrained to a set.
	virtual QString convertInternal2Display(Space internal)
	{
		return internal.toString();
	} ///< conversion from internal value to display value
	virtual QString convertRefObjectInternal2Display(QString internal)
	{
		return internal;
	}


};
typedef boost::shared_ptr<SpacePropertyBase> SpacePropertyBasePtr;

} // namespace cx

#endif // CXSPACEPROPERTYBASE_H_
