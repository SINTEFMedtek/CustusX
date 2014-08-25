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

#include "cxColorDataAdapterXml.h"
#include "cxVector3D.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"

namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
ColorDataAdapterXmlPtr ColorDataAdapterXml::initialize(const QString& uid, QString name, QString help, QColor value,
    QDomNode root)
{
    ColorDataAdapterXmlPtr retval(new ColorDataAdapterXml());
    retval->mUid = uid;
    retval->mName = name.isEmpty() ? uid : name;
    retval->mHelp = help;
    retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = string2color(retval->mStore.readValue(color2string(value)));
    return retval;
}

QString ColorDataAdapterXml::getDisplayName() const
{
    return mName;
}

QString ColorDataAdapterXml::getValueAsString() const
{
	return mValue.name();
}

void ColorDataAdapterXml::setValueFromString(QString value)
{
	QColor val(value);
	if(val.isValid())
		this->setValue(val);
	else
		reportError("Could not convert "+value+" to QColor.");
}

QString ColorDataAdapterXml::getUid() const
{
    return mUid;
}

QString ColorDataAdapterXml::getHelp() const
{
    return mHelp;
}

QColor ColorDataAdapterXml::getValue() const
{
    return mValue;
}

bool ColorDataAdapterXml::setValue(QColor val)
{
    if (val == mValue)
        return false;

    mValue = val;
	mStore.writeValue(color2string(val));
    emit valueWasSet();
    emit changed();
    return true;
}

} // namespace cx

