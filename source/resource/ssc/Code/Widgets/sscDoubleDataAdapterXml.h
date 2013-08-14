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
 * sscDoubleDataAdapterXml.h
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */

#ifndef SSCDOUBLEDATAADAPTERXML_H_
#define SSCDOUBLEDATAADAPTERXML_H_

#include <QDomElement>
#include <QStringList>
#include "sscDoubleRange.h"
#include "sscDoubleDataAdapter.h"
#include "sscXmlOptionItem.h"

namespace ssc
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
 *   \ingroup sscWidget
 */
class DoubleDataAdapterXml: public DoubleDataAdapter
{
Q_OBJECT
public:
	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static DoubleDataAdapterXmlPtr initialize(const QString& uid, QString name, QString help, double value,
		DoubleRange range, int decimals, QDomNode root = QDomNode());
	void setInternal2Display(double factor);

public:
	// inherited interface
	virtual QString getValueName() const;///< name of data entity. Used for display to user.
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

    virtual bool addSlider() const { return mAddSlider; }
    void setAddSlider(bool add) { mAddSlider = add; }

public:
	QString getUid() const;

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
    bool mAddSlider;
};

} // namespace ssc

#endif /* SSCDOUBLEDATAADAPTERXML_H_ */
