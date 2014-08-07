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

#include "cxDoublePairDataAdapterXml.h"
#include "cxTypeConversions.h"
#include "cxVector3D.h"

namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
DoublePairDataAdapterXmlPtr DoublePairDataAdapterXml::initialize(const QString& uid, QString name, QString help,
	DoubleRange range, int decimals, QDomNode root)
{
	DoublePairDataAdapterXmlPtr retval(new DoublePairDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = fromString(retval->mStore.readValue(qstring_cast(Eigen::Vector2d(0, 0))));
	retval->mDecimals = decimals;
	return retval;
}

DoublePairDataAdapterXml::DoublePairDataAdapterXml()
{
	mFactor = 1.0;
}

void DoublePairDataAdapterXml::setInternal2Display(double factor)
{
	mFactor = factor;
}

QString DoublePairDataAdapterXml::getUid() const
{
	return mUid;
}

QString DoublePairDataAdapterXml::getValueName() const
{
	return mName;
}

QString DoublePairDataAdapterXml::getHelp() const
{
	return mHelp;
}

Eigen::Vector2d DoublePairDataAdapterXml::getValue() const
{
	return mValue;
}

bool DoublePairDataAdapterXml::setValue(const Eigen::Vector2d& val)
{
	if (val == mValue)
		return false;

	mValue = val;
	mStore.writeValue(qstring_cast(val));
	emit valueWasSet();
	emit changed();
	return true;
}

DoubleRange DoublePairDataAdapterXml::getValueRange() const
{
	return mRange;
}

void DoublePairDataAdapterXml::setValueRange(DoubleRange range)
{
	mRange = range;
	emit changed();
}

int DoublePairDataAdapterXml::getValueDecimals() const
{
	return mDecimals;
}

} // namespace cx
