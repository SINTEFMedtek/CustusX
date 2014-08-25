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


#ifndef CXSTRINGDATAADAPTER_H_
#define CXSTRINGDATAADAPTER_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QComboBox>
#include "cxDataAdapter.h"

namespace cx
{

/**\brief Abstract interface for interaction with internal string-valued data
 *
 * The class provides a bridge between general user interface code and specific
 * data structures. An implementation connects to a single data value.
 *
 *
 * Minimal implementation:
  \verbatim
       virtual QString getDisplayName() const;
       virtual bool setValue(QString value);
       virtual QString getValue() const;
       void changed();
  \endverbatim
 *
 * By implementing these methods you can set and get values, and the data has a name.
 * The changed() signal is used to make sure the user interface is updated even when
 * data is changed by some other source.
 *
 *
 * For more control use the methods:
  \verbatim
       virtual QStringList getValueRange() const;
  \endverbatim
 *
 *
 * If there is a difference between the internal data representation and
 * how you want to present them, use:
 *
  \verbatim
       virtual QString convertInternal2Display(QString internal);
  \endverbatim
 *
 * When testing, or during development, you can use the StringDataAdapterNull
 * as a dummy implementation.
 *
 * \ingroup cx_resource_core_dataadapters
 */
class StringDataAdapter: public DataAdapter
{
	Q_OBJECT
public:
	virtual ~StringDataAdapter(){}

public:
	// basic methods
	virtual QString getDisplayName() const = 0;///< name of data entity. Used for display to user.
	virtual QString getValueAsString() const { return this->getValue(); }
	virtual void setValueFromString(QString value) { this->setValue(value); }
	virtual QString getUid() const { return this->getDisplayName()+"_uid"; }
	virtual bool setValue(const QString& value) = 0; ///< set the data value.
	virtual QString getValue() const = 0; ///< get the data value.

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
	virtual QStringList getValueRange() const
	{
		return QStringList();
	} /// range of value. Use if data is constrained to a set.
	virtual QString convertInternal2Display(QString internal)
	{
		return internal;
	} ///< conversion from internal value to display value
};
typedef boost::shared_ptr<StringDataAdapter> StringDataAdapterPtr;

/**
 * \brief Dummy implementation of the StringDataAdapter
 *
 * \author Christian Askeland, SINTEF
 */
class StringDataAdapterNull: public StringDataAdapter
{
Q_OBJECT
public:
	virtual ~StringDataAdapterNull()
	{
	}
	virtual QString getDisplayName() const
	{
		return "dummy";
	}
	virtual QString getValueAsString() const
	{
		return "dummy";
	}
	virtual void setValueFromString(QString value)
	{

	}
	virtual QString getUid() const
	{
		return "dummy_id";
	}
	virtual bool setValue(const QString& value)
	{
		return false;
	}
	virtual QString getValue() const
	{
		return "";
	}
};
} // namespace cx

#endif /* CXSTRINGDATAADAPTER_H_ */
