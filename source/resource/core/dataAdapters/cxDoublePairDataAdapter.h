// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXDOUBLESPANSLIDERADAPTER_H
#define CXDOUBLESPANSLIDERADAPTER_H

#include "cxDataAdapter.h"
#include "cxDoubleRange.h"
#include "cxVector3D.h"

namespace cx
{
/**
 *
 * \brief Abstract interface for interaction with internal data structure: A pair of doubles
 * \ingroup cx_resource_core_dataadapters
 *
 * \date Juli 31, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class DoublePairDataAdapter : public DataAdapter
{
Q_OBJECT
public:
	virtual ~DoublePairDataAdapter() {}
//	DoubleSpanSliderAdapter();

public:
	// basic methods
	virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
//	virtual bool setValue(const std::pair<double,double>& value) = 0; ///< set the data value.
	virtual bool setValue(const Eigen::Vector2d& value) = 0; ///< set the data value.
//	virtual std::pair<double,double> getValue() const = 0; ///< get the data value.
	virtual Eigen::Vector2d getValue() const = 0; ///< get the data value.

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
typedef boost::shared_ptr<DoublePairDataAdapter> DoublePairDataAdapterPtr;

/** Dummy implementation */
class DoubleSpanSliderAdapterNull: public DoublePairDataAdapter
{
Q_OBJECT

public:
	virtual ~DoubleSpanSliderAdapterNull()
	{
	}
	virtual QString getValueName() const
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
#endif // CXDOUBLESPANSLIDERADAPTER_H
