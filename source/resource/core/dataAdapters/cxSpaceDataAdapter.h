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
#ifndef CXSPACEDATAADAPTER_H
#define CXSPACEDATAADAPTER_H

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QComboBox>
#include "cxDataAdapter.h"
#include "cxCoordinateSystemHelpers.h"

namespace cx
{

/**
 * \ingroup cx_resource_core_dataadapters
 */
class cxResource_EXPORT SpaceDataAdapter: public DataAdapter
{
	Q_OBJECT
public:
	SpaceDataAdapter() {}
	virtual ~SpaceDataAdapter(){}

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

};
typedef boost::shared_ptr<SpaceDataAdapter> SpaceDataAdapterPtr;

} // namespace cx

#endif // CXSPACEDATAADAPTER_H
