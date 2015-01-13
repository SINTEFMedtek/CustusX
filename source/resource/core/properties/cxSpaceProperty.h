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
#ifndef CXSPACEPROPERTY_H_
#define CXSPACEPROPERTY_H_


#include "cxResourceExport.h"

#include <QDomElement>
#include <QStringList>
#include "cxSpacePropertyBase.h"
#include "cxXmlOptionItem.h"

namespace cx
{

typedef boost::shared_ptr<class SpaceProperty> SpacePropertyPtr;

/**
 * \ingroup cx_resource_core_dataadapters
 */
class cxResource_EXPORT SpaceProperty: public SpacePropertyBase
{
Q_OBJECT
public:
	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static SpacePropertyPtr initialize(const QString& uid, QString name, QString help, Space value=Space(),
		std::vector<Space> range=std::vector<Space>(), QDomNode root = QDomNode());
	void setSpaceProvider(SpaceProviderPtr provider);

public:
	// inherited interface
	virtual QString getDisplayName() const;///< name of data entity. Used for display to user.
	virtual QString getUid() const;
	virtual bool setValue(const Space& value); ///< set the data value.
	virtual Space getValue() const; ///< get the data value.
	virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual void setHelp(QString val);
	virtual std::vector<Space> getValueRange() const; /// range of value. Use if data is constrained to a set.
	virtual void setValueRange(std::vector<Space> range);
	virtual QString convertRefObjectInternal2Display(QString internal); ///< conversion from internal value to display value
	virtual void setRefObjectDisplayNames(std::map<QString, QString> names);

	virtual bool isReadOnly() const { return mIsReadOnly; }
	virtual bool getAllowOnlyValuesInRange() const { return mAllowOnlyValuesInRange; }
	void setReadOnly(bool val);


signals:
	void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private slots:
	void providerChangedSlot();
private:
	SpaceProperty();
	QString mName;
	QString mUid;
	QString mHelp;
	Space mValue;
	std::vector<Space> mRange;
	XmlOptionItem mStore;
	std::map<QString, QString> mDisplayNames;
	bool mIsReadOnly;
	bool mAllowOnlyValuesInRange;

	SpaceProviderPtr mProvider;
};

// --------------------------------------------------------
// --------------------------------------------------------


} // namespace cx

#endif // CXSPACEPROPERTY_H_
