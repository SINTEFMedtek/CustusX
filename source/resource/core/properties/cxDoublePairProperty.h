/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDOUBLEPAIRPROPERTY_H_
#define CXDOUBLEPAIRPROPERTY_H_

#include "cxResourceExport.h"

#include "cxDoublePairPropertyBase.h"
#include "cxXmlOptionItem.h"

namespace cx
{
typedef boost::shared_ptr<class DoublePairProperty> DoublePairPropertyPtr;

/**
 * \brief Implementation of DoublePairPropertyBase.
 *
 *  Represents one option of the double type.
 *  The data are stored within a xml document.
 *
 *  The option node has this layout:
	 \verbatim
		<option id="Processor" value="3.14"/>
	 \endverbatim
 *
 * \ingroup cx_resource_core_properties
 *
 * \date Juli 31, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT DoublePairProperty : public DoublePairPropertyBase
{
Q_OBJECT
public:
	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static DoublePairPropertyPtr initialize(const QString& uid, QString name, QString help,
																								DoubleRange range, int decimals, QDomNode root = QDomNode());
	void setInternal2Display(double factor);

public:
	// inherited interface
	virtual QString getDisplayName() const;///< name of data entity. Used for display to user.
	virtual QString getUid() const;
	virtual bool setValue(const Eigen::Vector2d& value); ///< set the data value.
	virtual Eigen::Vector2d getValue() const; ///< get the data value.
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
	DoublePairProperty();
	QString mName;
	QString mUid;
	QString mHelp;
	Eigen::Vector2d mValue;
	double mDecimals;
	DoubleRange mRange;
	XmlOptionItem mStore;
	double mFactor;
};

} // namespace cx
#endif // CXDOUBLEPAIRPROPERTY_H_
