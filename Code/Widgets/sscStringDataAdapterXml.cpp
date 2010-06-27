/*
 * sscStringDataAdapterXml.cpp
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */
#include "sscStringDataAdapterXml.h"

#include <iostream>
//#include <QtCore>
#include <QDomElement>
#include <QStringList>

//#include "sscTypeConversions.h"
//#include "sscMessageManager.h"

namespace ssc
{

QDomElement StringDataAdapterXml::findElemFromUid(const QString& uid, QDomNode root)
{
  QDomNodeList settings = root.childNodes();
  for (int i=0; i<settings.size(); ++i)
  {
	QDomElement item = settings.item(i).toElement();
    if (item.attribute("id")==uid)
      return item;
  }
  return QDomElement();
}

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
StringDataAdapterXmlPtr StringDataAdapterXml::initialize(const QString& uid,
    QString name,
    QString help,
    QString value,
    QStringList range,
    QDomNode root)
{
	StringDataAdapterXmlPtr retval(new StringDataAdapterXml());

	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mValue = value;
	retval->mRange = range;
	retval->mRoot = root.toElement();

	// read value is present
	QDomElement item = retval->findElemFromUid(uid, root);
	if (!item.isNull() &&item.hasAttribute("value"))
	{
		retval->mValue = item.attribute("value");
	}

	return retval;
}

QString StringDataAdapterXml::getUid() const
{
	return mUid;
}

QString StringDataAdapterXml::getValueName() const
{
	return mName;
}

QString StringDataAdapterXml::getHelp() const
{
	return mHelp;
}

QString StringDataAdapterXml::getValue() const
{
  return mValue;
}

void StringDataAdapterXml::writeValue(const QString& val)
{
	QDomElement item = findElemFromUid(getUid(), mRoot);
  // create option if not present
  if (item.isNull())
  {
    item = mRoot.ownerDocument().createElement("option");
    item.setAttribute("id", getUid());
    mRoot.appendChild(item);
  }
  item.setAttribute("value", val);
}

bool StringDataAdapterXml::setValue(const QString& val)
{
	if (val==mValue)
		return false;

	mValue = val;
	this->writeValue(val);
	emit valueWasSet();
	emit changed();
	return true;
}

QStringList StringDataAdapterXml::getValueRange() const
{
	return mRange;
}



} // namespace ssc
