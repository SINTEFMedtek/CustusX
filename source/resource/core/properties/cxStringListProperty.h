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

#ifndef CXSTRINGLISTPROPERTY_H
#define CXSTRINGLISTPROPERTY_H

#include "cxResourceExport.h"

#include <QDomElement>
#include <QStringList>
#include "cxStringPropertyBase.h"
#include "cxXmlOptionItem.h"

namespace cx
{

typedef boost::shared_ptr<class StringListProperty> StringListPropertyPtr;

/**
 * \brief Represents one collection of strings.
 *
 * The collection can be selected from a range.
 * The data can be stored within a xml document.
 *
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT StringListProperty: public Property
{
Q_OBJECT
public:
	static StringListPropertyPtr initialize(const QString& uid, QString name, QString help, QStringList value, QStringList range, QDomNode root = QDomNode());

public:
	// inherited interface
	virtual QString getDisplayName() const;///< name of data entity. Used for display to user.
	virtual QString getUid() const;
	virtual QVariant getValueAsVariant() const;
	virtual void setValueFromVariant(QVariant val);

	virtual bool setValue(const QStringList& value); ///< set the data value.
	virtual QStringList getValue() const; ///< get the data value.
	virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual void setHelp(QString val);
	virtual QStringList getValueRange() const; /// range that values can be selected from
	virtual void setValueRange(QStringList range);
	virtual QString convertInternal2Display(QString internal); ///< conversion from internal value to display value
	virtual void setDisplayNames(std::map<QString, QString> names);
	virtual std::map<QString, QString> getDisplayNames() const { return mDisplayNames; }

	virtual void setDisplayName(QString val);

private:
	StringListProperty();
	QString mName;
	QString mUid;
	QString mHelp;
	QStringList mValue;
	QStringList mRange;
	XmlOptionItem mStore;
	std::map<QString, QString> mDisplayNames;
};

} // namespace cx

#endif // CXSTRINGLISTPROPERTY_H
