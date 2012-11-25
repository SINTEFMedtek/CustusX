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

#include "sscColorDataAdapterXml.h"
#include "sscVector3D.h"
#include "sscTypeConversions.h"

namespace ssc
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
    retval->mValue = QColor(retval->string2color(retval->mStore.readValue(retval->color2string(value))));
    return retval;
}

QString ColorDataAdapterXml::color2string(QColor color) const
{
    return qstring_cast(Eigen::Vector4f(color.redF(), color.greenF(), color.blueF(), color.alphaF()));
}

QColor ColorDataAdapterXml::string2color(QString input) const
{
    QStringList c = input.split(" ");
    if (c.size()<4)
        return QColor("green");
    bool ok;
    return QColor::fromRgbF(c[0].toDouble(&ok), c[1].toDouble(&ok), c[2].toDouble(&ok), c[3].toDouble(&ok));
}

QString ColorDataAdapterXml::getUid() const
{
    return mUid;
}

QString ColorDataAdapterXml::getValueName() const
{
    return mName;
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
    mStore.writeValue(this->color2string(val));
    emit valueWasSet();
    emit changed();
    return true;
}

} // namespace ssc

