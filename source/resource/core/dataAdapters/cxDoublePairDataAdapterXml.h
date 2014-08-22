// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXDOUBLEPAIRDATAADAPTERXML_H
#define CXDOUBLEPAIRDATAADAPTERXML_H

#include "cxDoublePairDataAdapter.h"
#include "cxXmlOptionItem.h"

namespace cx
{
typedef boost::shared_ptr<class DoublePairDataAdapterXml> DoublePairDataAdapterXmlPtr;

/**
 * \brief Implementation of DoublePairDataAdapter.
 *
 *  Represents one option of the double type.
 *  The data are stored within a xml document.
 *
 *  The option node has this layout:
	 \verbatim
		<option id="Processor" value="3.14"/>
	 \endverbatim
 *
 * \ingroup cx_resource_core_dataadapters
 *
 * \date Juli 31, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class DoublePairDataAdapterXml : public DoublePairDataAdapter
{
Q_OBJECT
public:
	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static DoublePairDataAdapterXmlPtr initialize(const QString& uid, QString name, QString help,
																								DoubleRange range, int decimals, QDomNode root = QDomNode());
	void setInternal2Display(double factor);

public:
	// inherited interface
	virtual QString getDisplayName() const;///< name of data entity. Used for display to user.
	virtual QString getValueAsString() const;
	virtual void setValueFromString(QString value);
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
	DoublePairDataAdapterXml();
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
#endif // CXDOUBLEPAIRDATAADAPTERXML_H
