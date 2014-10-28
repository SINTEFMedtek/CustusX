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
 * sscVector3DDataAdapter.h
 *
 *  Created on: Jul 25, 2011
 *      Author: christiana
 */

#ifndef CXVECTOR3DDATAADAPTER_H_
#define CXVECTOR3DDATAADAPTER_H_

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include "cxDoubleRange.h"
#include "cxDataAdapter.h"
#include "cxVector3D.h"

namespace cx
{

/**\brief Abstract interface for interaction with internal Vector3D-valued data
 *
 *  Refer to DoubleDataAdapter for a description.
 *
 * \ingroup cx_resource_core_dataadapters
 */
class cxResource_EXPORT Vector3DDataAdapter: public DataAdapter
{
Q_OBJECT
public:
	virtual ~Vector3DDataAdapter()
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
typedef boost::shared_ptr<Vector3DDataAdapter> Vector3DDataAdapterPtr;

/** Dummy implementation */
class cxResource_EXPORT Vector3DDataAdapterNull: public Vector3DDataAdapter
{
Q_OBJECT

public:
	virtual ~Vector3DDataAdapterNull()
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

#endif /* CXVECTOR3DDATAADAPTER_H_ */
