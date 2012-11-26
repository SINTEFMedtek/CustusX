// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscDoubleDataAdapter.h
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#ifndef SSCDOUBLEDATAADAPTER_H_
#define SSCDOUBLEDATAADAPTER_H_

#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include "sscDoubleRange.h"
#include "sscDataAdapter.h"

namespace ssc
{

/**\brief Abstract interface for interaction with internal double-valued data
 *
 * The class provides a bridge between general user interface code and specific
 * data structures. An implementation connects to a single data value.
 *
 *
 * Minimal implementation:
   \verbatim
       virtual QString getValueName() const;
       virtual bool setValue(double value);
       virtual double getValue() const;
       void changed();
   \endverbatim
 * By implementing these methods you can set and get values, and the data has a name.
 * The changed() signal is used to make sure the user interface is updated even when
 * data is changed by some other source.
 *
 *
 * For more control use the methods:
   \verbatim
       virtual DoubleRange getValueRange() const;
       virtual int getValueDecimals() const;
   \endverbatim
 *
 *
 * If there is a difference between the internal data representation and
 * how you want to present them, use:
   \verbatim
       virtual double convertInternal2Display(double internal);
       virtual double convertDisplay2Internal(double display);
   \endverbatim
 *
 * When testing, or during development, you can use the DoubleDataAdapterNull
 * as a dummy implementation.
 *
 * \ingroup sscWidget
 */
class DoubleDataAdapter: public DataAdapter
{
Q_OBJECT
public:
	virtual ~DoubleDataAdapter()
	{
	}

public:
	// basic methods
	virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
	virtual bool setValue(double value) = 0; ///< set the data value.
	virtual double getValue() const = 0; ///< get the data value.
    virtual bool addSlider() const { return false; } ///< request gui to add a slider in addition to editbox etc.

public:
	// optional methods
	virtual QString getHelp() const
	{
		return QString();
	} ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual DoubleRange getValueRange() const
	{
		return DoubleRange(0, 1, 0.01);
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

	//virtual double legendStep() const {return 0.2;} ///< step between each pos on the legend
	//virtual void connectValueSignals(bool on) {} ///< set object to emit changed() when applicable

	//  signals:
	//  void changed(); ///< emit when the underlying data value is changed: The user interface will be updated.
};
typedef boost::shared_ptr<DoubleDataAdapter> DoubleDataAdapterPtr;

/** Dummy implementation */
class DoubleDataAdapterNull: public DoubleDataAdapter
{
Q_OBJECT

public:
	virtual ~DoubleDataAdapterNull()
	{
	}
	virtual QString getValueName() const
	{
		return "dummy";
	}
	virtual bool setValue(double value)
	{
		return false;
	}
	virtual double getValue() const
	{
		return 0;
	}
	virtual void connectValueSignals(bool on)
	{
	}
};

}

#endif /* SSCDOUBLEDATAADAPTER_H_ */
