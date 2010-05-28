/*
 * sscXmlOptionItem.h
 *
 *  Created on: May 28, 2010
 *      Author: christiana
 */
#ifndef SSCXMLOPTIONITEM_H_
#define SSCXMLOPTIONITEM_H_

#include <QDomElement>
#include <QStringList>

namespace ssc
{

/** Represents one option of the string type.
 *  The data are stored within a xml document.
 *
 *  The option node has this layout:
 *   <option>
 *     <id>Processor</id>
 *     <name>Nice name for Processor, optional</name>
 *     <help>Preferred type of processor to use during reconstruction</help>
 *     <value>CPU</value>
 *     <type>string</type>
 *     <range>"GPU" "CPU"</range>
 *   </option>
 */
class StringOptionItem
{
public:
  /** find and return the setting with id==uid among the children of root.
   */
  static StringOptionItem fromName(const QString& uid, QDomNode root)
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
  static void initialize(const QString& uid,
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

  StringOptionItem(QDomElement element) : mElement(element) {}
  QString getId() const
  {
    return mElement.attribute("id");
    //return mElement.namedItem("id").toElement().text();
  }
  QString getName() const
  {
    QString name = mElement.namedItem("name").toElement().text();
    if (!name.isEmpty())
      return name;
    return this->getId();
  }
  QString getHelp() const
  {
    return mElement.namedItem("help").toElement().text();
  }
  QString getValue() const
  {
    return mElement.namedItem("value").toElement().text();
  }
  void setValue(const QString& val)
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
  QStringList getRange() const
  {
    QString str = mElement.namedItem("range").toElement().text();
    // split sequences of type "a" "bb" "ff f"
    QStringList retval = str.split(QRegExp("\"|\"\\s+\""), QString::SkipEmptyParts);
    return retval;
  }
private:
  QDomElement mElement;

  /** Get a text node from child with name, create if not existent.
   */
  QDomText safeGetTextNode(QString name)
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
};

} // namespace ssc
#endif /* SSCXMLOPTIONITEM_H_ */
