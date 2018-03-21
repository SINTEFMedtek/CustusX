/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxColorProperty.h"
#include "cxVector3D.h"
#include "cxTypeConversions.h"


namespace cx
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
ColorPropertyPtr ColorProperty::initialize(const QString& uid, QString name, QString help, QColor value,
    QDomNode root)
{
    ColorPropertyPtr retval(new ColorProperty());
    retval->mUid = uid;
    retval->mName = name.isEmpty() ? uid : name;
    retval->mHelp = help;
    retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = string2color(retval->mStore.readValue(color2string(value)));
    return retval;
}

QString ColorProperty::getDisplayName() const
{
    return mName;
}

QString ColorProperty::getUid() const
{
    return mUid;
}

QString ColorProperty::getHelp() const
{
    return mHelp;
}

QColor ColorProperty::getValue() const
{
    return mValue;
}

bool ColorProperty::setValue(QColor val)
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

