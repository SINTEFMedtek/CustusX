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
 * sscBoolDataAdapterXml.h
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#ifndef SSCBOOLDATAADAPTERXML_H_
#define SSCBOOLDATAADAPTERXML_H_

#include <QDomElement>

#include "sscBoolDataAdapter.h"
#include "sscXmlOptionItem.h"

namespace ssc
{

typedef boost::shared_ptr<class BoolDataAdapterXml> BoolDataAdapterXmlPtr;

/**\brief DataAdapter for boolean values.
 *
 * \ingroup sscWidget
 */
class BoolDataAdapterXml: public BoolDataAdapter
{
Q_OBJECT
public:
	virtual ~BoolDataAdapterXml()
	{
	}

	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static BoolDataAdapterXmlPtr initialize(const QString& uid, QString name, QString help, bool value, QDomNode root =
		QDomNode());

public:
	// basic methods
	virtual QString getValueName() const; ///< name of data entity. Used for display to user.
	virtual bool setValue(bool value); ///< set the data value.
	virtual bool getValue() const; ///< get the data value.

public:
	// optional methods
	virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
	QString getUid() const;
    void setHelp(QString val);
    /*
	virtual bool getEnabled() const; ///< Get the enabled/disabled state of the dataadapter.

public slots:
	virtual bool setEnabled(bool enabled); ///< Set the enabled/disabled state of the dataadapter.
*/
signals:
	void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
	QString mName;
	QString mUid;
	QString mHelp;
	bool mValue;
	XmlOptionItem mStore;

};

}

#endif /* SSCBOOLDATAADAPTERXML_H_ */
