/*
 * sscDoubleDataAdapterXml.cpp
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */
#include "sscDoubleDataAdapterXml.h"
//#include <QDomElement>

#include <iostream>
//#include <QtCore>
#include <QDomElement>
#include <QStringList>

//#include "sscTypeConversions.h"
//#include "sscMessageManager.h"

namespace ssc
{

QDomElement DoubleDataAdapterXml::findElemFromUid(const QString& uid, QDomNode root)
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
DoubleDataAdapterXmlPtr DoubleDataAdapterXml::initialize(const QString& uid,
    QString name,
    QString help,
    double value,
    DoubleRange range,
    int decimals,
    QDomNode root)
{
	DoubleDataAdapterXmlPtr retval(new DoubleDataAdapterXml());

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
		retval->mValue = item.attribute("value").toDouble();
	}

	return retval;
}

QString DoubleDataAdapterXml::getUid() const
{
	return mUid;
}

QString DoubleDataAdapterXml::getValueName() const
{
	return mName;
}

QString DoubleDataAdapterXml::getHelp() const
{
	return mHelp;
}

double DoubleDataAdapterXml::getValue() const
{
  return mValue;
}

void DoubleDataAdapterXml::writeValue(const QString& val)
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

bool DoubleDataAdapterXml::setValue(double val)
{
	if (val==mValue)
		return false;

	mValue = val;
	this->writeValue(QString::number(val));
	emit valueWasSet();
	emit changed();
	return true;
}

DoubleRange DoubleDataAdapterXml::getValueRange() const
{
	return mRange;
}

int DoubleDataAdapterXml::getValueDecimals() const
{
	return mDecimals;
}


} // namespace ssc
