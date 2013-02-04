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
 * sscStringDataAdapterXml.h
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */

#ifndef SSCSTRINGDATAADAPTERXML_H_
#define SSCSTRINGDATAADAPTERXML_H_

#include <QDomElement>
#include <QStringList>
#include "sscStringDataAdapter.h"
#include "sscXmlOptionItem.h"

namespace ssc
{

typedef boost::shared_ptr<class StringDataAdapterXml> StringDataAdapterXmlPtr;

/**\brief Represents one option of the string type.
 *  The data are stored within a xml document.
 *
 *  The option node has this layout:
   \verbatim
    <option id="Processor" value="3.14"/>
   \endverbatim
 *
 *   \ingroup sscWidget
 */
class StringDataAdapterXml: public StringDataAdapter
{
Q_OBJECT
public:
	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static StringDataAdapterXmlPtr initialize(const QString& uid, QString name, QString help, QString value,
		QStringList range, QDomNode root);
	static StringDataAdapterXmlPtr initialize(const QString& uid, QString name, QString help, QString value, QDomNode root);

public:
	// inherited interface
	virtual QString getValueName() const;///< name of data entity. Used for display to user.
	virtual bool setValue(const QString& value); ///< set the data value.
	virtual QString getValue() const; ///< get the data value.
	virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
    virtual void setHelp(QString val);
	virtual QStringList getValueRange() const; /// range of value. Use if data is constrained to a set.
	virtual void setValueRange(QStringList range);
	virtual QString convertInternal2Display(QString internal); ///< conversion from internal value to display value
	virtual void setDisplayNames(std::map<QString, QString> names);

	virtual bool isReadOnly() const { return mIsReadOnly; }
	virtual bool getAllowOnlyValuesInRange() const { return mAllowOnlyValuesInRange; }
	void setReadOnly(bool val);

public:
	QString getUid() const;

signals:
	void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
	StringDataAdapterXml();
	QString mName;
	QString mUid;
	QString mHelp;
	QString mValue;
	QStringList mRange;
	XmlOptionItem mStore;
	std::map<QString, QString> mDisplayNames;
	bool mIsReadOnly;
	bool mAllowOnlyValuesInRange;
};

// --------------------------------------------------------
// --------------------------------------------------------


} // namespace ssc

#endif /* SSCSTRINGDATAADAPTERXML_H_ */
