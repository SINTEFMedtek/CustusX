/*
 * sscXmlOptionItem.cpp
 *
 *  Created on: May 28, 2010
 *      Author: christiana
 */
#include "sscXmlOptionItem.h"

#include <iostream>
#include <QtCore>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"

namespace ssc
{

StringOptionItem StringOptionItem::fromName(const QString& uid, QDomNode root)
{
  QDomNodeList settings = root.childNodes();
  for (int i=0; i<settings.size(); ++i)
  {
    StringOptionItem item(settings.item(i).toElement());
    //std::cout << "getnamed("<<uid<<") "<< item.getId() << std::endl;
    if (item.getId()==uid)
      return item;
  }
  return StringOptionItem(QDomElement());
}
/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
void StringOptionItem::initialize(const QString& uid,
    QString name,
    QString help,
    QString value,
    QString range,
    QDomNode root)
{
  StringOptionItem item = fromName(uid, root);
  // create option if not present
  if (item.mElement.isNull())
  {
    QDomElement node =root.ownerDocument().createElement("option");
    root.appendChild(node);
    node.setAttribute("id", uid);
    item = fromName(uid, root);
  }
  // set default value if not present
  if (item.mElement.namedItem("value").isNull())
  {
    QDomText text = item.safeGetTextNode("value");
    text.setData(value);
  }
  // force set all the rest
  if (!name.isEmpty())
    item.safeGetTextNode("name").setData(name);
  if (!help.isEmpty())
    item.safeGetTextNode("help").setData(help);
  if (!range.isEmpty())
    item.safeGetTextNode("range").setData(range);
  item.safeGetTextNode("type").setData("string");
}

StringOptionItem::StringOptionItem(QDomElement element) : mElement(element)
{
}

QString StringOptionItem::getId() const
{
  return mElement.attribute("id");
  //return mElement.namedItem("id").toElement().text();
}

QString StringOptionItem::getName() const
{
  QString name = mElement.namedItem("name").toElement().text();
  if (!name.isEmpty())
    return name;
  return this->getId();
}

QString StringOptionItem::getHelp() const
{
  return mElement.namedItem("help").toElement().text();
}

QString StringOptionItem::getValue() const
{
  return mElement.namedItem("value").toElement().text();
}

void StringOptionItem::setValue(const QString& val)
{
  QDomText text = this->safeGetTextNode("value");
//    QDomText text = mElement.namedItem("value").firstChild().toText();
//    if (text.isNull())
//    {
//      std::cout << "setvalue: null" << std::endl;
//      text = mElement.ownerDocument().createTextNode("");
//      mElement.namedItem("value").appendChild(text);
//    }
  std::cout << "setvalue: " << val.toStdString() << std::endl;
  text.setData(val);
}

QStringList StringOptionItem::getRange() const
{
  QString str = mElement.namedItem("range").toElement().text();
  // split sequences of type "a" "bb" "ff f"
  QStringList retval = str.split(QRegExp("\"|\"\\s+\""), QString::SkipEmptyParts);
  return retval;
}

QDomText StringOptionItem::safeGetTextNode(QString name)
{
  QDomElement node = mElement.namedItem(name).toElement();
  if (node.isNull())
  {
    node = mElement.ownerDocument().createElement(name);
    mElement.appendChild(node);
  }
  QDomText text = node.firstChild().toText();
  if (text.isNull())
  {
    text = mElement.ownerDocument().createTextNode("");
    node.appendChild(text);
  }
  return text;
}



//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

XmlOptionFile::XmlOptionFile()
{
}

XmlOptionFile::XmlOptionFile(QString filename, QDomDocument def) :
    mFilename(filename),
    mDocument(def)
{
  this->load();
}

StringOptionItem XmlOptionFile::getStringOption(const QString& uid)
{
  return StringOptionItem::fromName(uid, this->getElement());
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
