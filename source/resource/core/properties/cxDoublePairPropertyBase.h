/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDOUBLEPAIRPROPERTYBASE_H_
#define CXDOUBLEPAIRPROPERTYBASE_H_

#include "cxResourceExport.h"

#include "cxProperty.h"
#include "cxDoubleRange.h"
#include "cxMathBase.h"
#include "cxTypeConversions.h"
#include "cxVector3D.h"

namespace cx
{
/**
 *
 * \brief Abstract interface for interaction with internal data structure: A pair of doubles
 * \ingroup cx_resource_core_properties
 *
 * \date Juli 31, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT DoublePairPropertyBase : public Property
{
Q_OBJECT
public:
	virtual ~DoublePairPropertyBase() {}
//	DoubleSpanSliderAdapter();

public:
	// basic methods
	virtual QString getDisplayName() const = 0; ///< name of data entity. Used for display to user.
//	virtual bool setValue(const std::pair<double,double>& value) = 0; ///< set the data value.
	virtual bool setValue(const Eigen::Vector2d& value) = 0; ///< set the data value.
//	virtual std::pair<double,double> getValue() const = 0; ///< get the data value.
	virtual Eigen::Vector2d getValue() const = 0; ///< get the data value.

	virtual void setValueRange(DoubleRange range) = 0;

	virtual QVariant getValueAsVariant() const
	{
		QString val = qstring_cast(this->getValue());
		return QVariant(val);
	}

	virtual void setValueFromVariant(QVariant value)
	{
		Eigen::Vector2d val = fromString(value.toString());
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
typedef boost::shared_ptr<DoublePairPropertyBase> DoublePairPropertyBasePtr;

/** Dummy implementation */
class cxResource_EXPORT DoubleSpanSliderAdapterNull: public DoublePairPropertyBase
{
Q_OBJECT

public:
	virtual ~DoubleSpanSliderAdapterNull()
	{
	}
	virtual QString getDisplayName() const
	{
		return "dummy";
	}
//	virtual bool setValue(const std::pair<double,double>& value)
	virtual bool setValue(const Eigen::Vector2d& value)
	{
		return false;
	}
//	virtual std::pair<double,double> getValue() const
	virtual Eigen::Vector2d getValue() const
	{
//		return std::make_pair(0,0);
		return Eigen::Vector2d(0, 0);
	}
	virtual void connectValueSignals(bool on)
	{
	}
};

} // namespace cx
#endif // CXDOUBLEPAIRPROPERTYBASE_H_
