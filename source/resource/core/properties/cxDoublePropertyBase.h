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


/*
 * sscDoubleProperty.h
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#ifndef CXDOUBLEPROPERTYBASE_H_
#define CXDOUBLEPROPERTYBASE_H_

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include "cxDoubleRange.h"
#include "cxProperty.h"

namespace cx
{

/** Abstract interface for interaction with internal double-valued data
 *
 * The class provides a bridge between general user interface code and specific
 * data structures. An implementation connects to a single data value.
 *
 *
 * Minimal implementation:
   \verbatim
       virtual QString getDisplayName() const;
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
 * When testing, or during development, you can use the DoublePropertyNull
 * as a dummy implementation.
 *
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT DoublePropertyBase: public Property
{
Q_OBJECT
public:
	DoublePropertyBase() : mGuiRepresentation(grSPINBOX){}
	virtual ~DoublePropertyBase(){}

public:
	enum GuiRepresentation
	{
		grSPINBOX,
		grSLIDER,
		grDIAL
	};

	// basic methods
	virtual QString getDisplayName() const = 0; ///< name of data entity. Used for display to user.

	virtual QVariant getValueAsVariant() const
	{
		return QVariant(this->getValue());
	}
	virtual void setValueFromVariant(QVariant val)
	{
		this->setValue(val.toDouble());
	}


	virtual QString getUid() const { return this->getDisplayName()+"_uid"; }
	virtual bool setValue(double value) = 0; ///< set the data value.
	virtual double getValue() const = 0; ///< get the data value.

	virtual void setGuiRepresentation(GuiRepresentation type) { mGuiRepresentation = type; };
	virtual GuiRepresentation getGuiRepresentation() { return mGuiRepresentation; };

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

protected:
    GuiRepresentation mGuiRepresentation;

};
typedef boost::shared_ptr<DoublePropertyBase> DoublePropertyBasePtr;

/** Dummy implementation */
class cxResource_EXPORT DoublePropertyNull: public DoublePropertyBase
{
Q_OBJECT

public:
	virtual ~DoublePropertyNull()
	{
	}
	virtual QString getDisplayName() const
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

#endif /* CXDOUBLEPROPERTYBASE_H_ */
