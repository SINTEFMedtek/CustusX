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
 * sscDoubleDataAdapterXml.h
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */

#ifndef CXDOUBLEPROPERTY_H_
#define CXDOUBLEPROPERTY_H_

#include "cxResourceExport.h"

#include <QDomElement>
#include <QStringList>
#include "cxDoubleRange.h"
#include "cxDoublePropertyBase.h"
#include "cxXmlOptionItem.h"

namespace cx
{

typedef boost::shared_ptr<class DoubleDataAdapterXml> DoubleDataAdapterXmlPtr;

/**\brief Implementation of DoubleDataAdapter.
 *
 *  Represents one option of the double type.
 *  The data are stored within a xml document.
 *
 *  The option node has this layout:
   \verbatim
    <option id="Processor" value="3.14"/>
   \endverbatim
 *
 * \ingroup cx_resource_core_dataadapters
 */
class cxResource_EXPORT DoubleDataAdapterXml: public DoubleDataAdapter
{

	Q_OBJECT
public:
	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static DoubleDataAdapterXmlPtr initialize(const QString& uid, QString name, QString help, double value, DoubleRange range, int decimals, QDomNode root = QDomNode());
	void setInternal2Display(double factor);

public:
	// inherited interface
	virtual QString getDisplayName() const;///< name of data entity. Used for display to user.
	virtual QString getUid() const;
	virtual bool setValue(double value); ///< set the data value.
	virtual double getValue() const; ///< get the data value.
	virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual DoubleRange getValueRange() const; /// range of value. Use if data is constrained to a set.
	virtual void setValueRange(DoubleRange range);
	virtual int getValueDecimals() const; ///< number of relevant decimals in value
	virtual double convertInternal2Display(double internal)
	{
		return mFactor * internal;
	} ///< conversion from internal value to display value (for example between 0..1 and percent)
	virtual double convertDisplay2Internal(double display)
	{
		return display / mFactor;
	} ///< conversion from internal value to display value


signals:
	void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
	DoubleDataAdapterXml();
	QString mName;
	QString mUid;
	QString mHelp;
	double mValue;
	double mDecimals;
	DoubleRange mRange;
	XmlOptionItem mStore;
	double mFactor;
};

} // namespace cx

#endif /* CXDOUBLEPROPERTY_H_ */
