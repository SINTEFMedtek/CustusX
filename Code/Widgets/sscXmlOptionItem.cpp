/*
 * sscXmlOptionItem.cpp
 *
 *  Created on: May 28, 2010
 *      Author: christiana
 */
#include "sscXmlOptionItem.h"

#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDomElement>
#include <QStringList>

#include "sscMessageManager.h"

namespace ssc
{


XmlOptionItem::XmlOptionItem(const QString& uid,
	      QDomElement root) :
	      mUid(uid),
	      mRoot(root)
{

}

QString XmlOptionItem::readValue(const QString& defval) const
{
	// read value is present
	QDomElement item = this->findElemFromUid(mUid, mRoot);
	if (!item.isNull() &&item.hasAttribute("value"))
	{
		return item.attribute("value");
	}
	return defval;
}

void XmlOptionItem::writeValue(const QString& val)
{
	QDomElement item = findElemFromUid(mUid, mRoot);
  // create option if not present
  if (item.isNull())
  {
    item = mRoot.ownerDocument().createElement("option");
    item.setAttribute("id", mUid);
    mRoot.appendChild(item);
  }
  item.setAttribute("value", val);
}

QDomElement XmlOptionItem::findElemFromUid(const QString& uid, QDomNode root) const
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

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------


XmlOptionFile::XmlOptionFile()
{
}

XmlOptionFile::XmlOptionFile(QString filename, QDomDocument def) :
    mFilename(filename),
    mDocument(def)
{
  this->load();
}

/**return an element child of parent. Create if not existing.
 */
QDomElement XmlOptionFile::safeGetElement(QDomElement parent, QString childName)
{
  QDomElement child = parent.namedItem(childName).toElement();

  if (child.isNull())
  {
    child = mDocument.createElement(childName);
    parent.appendChild(child);
  }

  return child;
}

QDomElement XmlOptionFile::getElement()
{
  return mDocument.documentElement();
}

QDomElement XmlOptionFile::getElement(QString level1)
{
  QDomElement elem1 = this->safeGetElement(mDocument.documentElement(), level1);
  return elem1;
}

QDomElement XmlOptionFile::getElement(QString level1, QString level2)
{
  QDomElement elem1 = this->safeGetElement(mDocument.documentElement(), level1);
  QDomElement elem2 = this->safeGetElement(elem1, level2);
  return elem2;
}

void XmlOptionFile::save()
{
  QFile file(mFilename);
  if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QTextStream stream(&file);
    stream << mDocument.toString();
    file.close();
    ssc::messageManager()->sendInfo("Created "+file.fileName().toStdString());
  }
  else
  {
    ssc::messageManager()->sendError("Could not open "+file.fileName().toStdString()
                               +" Error: "+file.errorString().toStdString());
  }
}

void XmlOptionFile::load()
{
  QFile file(mFilename);
  if (!file.open(QIODevice::ReadOnly))
  {
    // ok to not find file - we have nice defaults.
    //ssc::messageManager()->sendWarning("file not found: "+ QString(defPath+filename).toStdString());
  }
  else
  {
    QDomDocument loadedDoc;
    QString error;
    int line,col;
    if (!loadedDoc.setContent(&file, &error,&line,&col))
    {
      ssc::messageManager()->sendWarning("error setting xml content ["
                                         + string_cast(line) +  ","
                                         + string_cast(col) + "]"
                                         + string_cast(error) );
    }
    file.close();
    mDocument = loadedDoc;
  }

}


} // namespace ssc
