/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXVECTOR3DPROPERTYBASE_H_
#define CXVECTOR3DPROPERTYBASE_H_

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include "cxDoubleRange.h"
#include "cxProperty.h"
#include "cxVector3D.h"

namespace cx
{

/**\brief Abstract interface for interaction with internal Vector3D-valued data
 *
 *  Refer to DoubleProperty for a description.
 *
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT Vector3DPropertyBase: public Property
{
Q_OBJECT
public:
	virtual ~Vector3DPropertyBase()
	{
	}

public:
	// basic methods
	virtual QString getDisplayName() const = 0; ///< name of data entity. Used for display to user.
	virtual bool setValue(const Vector3D& value) = 0; ///< set the data value.
	virtual Vector3D getValue() const = 0; ///< get the data value.

	virtual QVariant getValueAsVariant() const
	{
		QString val = prettyFormat(this->getValue(), this->getValueDecimals());
		return QVariant(val);
	//	return QVariant(this->getValue());
	}

	virtual void setValueFromVariant(QVariant value)
	{
		Vector3D val = Vector3D::fromString(value.toString());
		this->setValue(val);
	}

public:
	// optional methods
	virtual QString getHelp() const
	{
		return QString();
	} ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual DoubleRange getValueRange() const
	{
		return DoubleRange(-1000, 1000, 0.1);
	} /// range of value
	virtual double convertInternal2Display(double internal)
	{
		return internal;
	} ///< conversion from internal value to display value (for example between 0..1 and percent)
	virtual double convertDisplay2Internal(double display)
	{
		return display;
	} ///< conversion from internal value to display value
	virtual int getValueDecimals() const
	{
		return 0;
	} ///< number of relevant decimals in value
};
typedef boost::shared_ptr<Vector3DPropertyBase> Vector3DPropertyBasePtr;

/** Dummy implementation */
class cxResource_EXPORT Vector3DPropertyNull: public Vector3DPropertyBase
{
Q_OBJECT

public:
	virtual ~Vector3DPropertyNull()
	{
	}
	virtual QString getDisplayName() const
	{
		return "dummy";
	}
	virtual bool setValue(const Vector3D& value)
	{
		return false;
	}
	virtual Vector3D getValue() const
	{
		return Vector3D(0, 0, 0);
	}
	virtual void connectValueSignals(bool on)
	{
	}
};

}

#endif /* CXVECTOR3DPROPERTYBASE_H_ */
