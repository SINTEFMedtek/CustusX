/*
 * sscBoolDataAdapterXml.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#include <sscBoolDataAdapterXml.h>

namespace ssc
{

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
BoolDataAdapterXmlPtr BoolDataAdapterXml::initialize(const QString& uid,
    QString name,
    QString help,
    bool value,
    QDomNode root)
{
  BoolDataAdapterXmlPtr retval(new BoolDataAdapterXml());
  retval->mUid = uid;
  retval->mName = name.isEmpty() ? uid : name;
  retval->mHelp = help;
  retval->mStore = XmlOptionItem(uid, root.toElement());
  retval->mValue = retval->mStore.readValue(QString::number(value)).toInt();
  return retval;
}



QString BoolDataAdapterXml::getUid() const
{
  return mUid;
}

QString BoolDataAdapterXml::getValueName() const
{
  return mName;
}

QString BoolDataAdapterXml::getHelp() const
{
  return mHelp;
}

bool BoolDataAdapterXml::getValue() const
{
  return mValue;
}

bool BoolDataAdapterXml::setValue(bool val)
{
  if (val==mValue)
    return false;

  mValue = val;
  mStore.writeValue(QString(val));
  emit valueWasSet();
  emit changed();
  return true;
}

}
