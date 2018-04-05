/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
 * \ingroup cx_resource_core_properties
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
