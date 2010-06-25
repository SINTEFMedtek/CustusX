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
  static StringOptionItem fromName(const QString& uid, QDomNode root);

  /** Make sure one given option exists witin root.
   * If not present, fill inn the input defaults.
   */
  static void initialize(const QString& uid,
      QString name,
      QString help,
      QString value,
      QString range,
      QDomNode root);

public:
  StringOptionItem(QDomElement element);
  QString getId() const;
  QString getName() const;
  QString getHelp() const;
  QString getValue() const;
  void setValue(const QString& val);
  QStringList getRange() const;

private:
  QDomElement mElement;
  /** Get a text node from child with name, create if not existent.
   */
  QDomText safeGetTextNode(QString name);
};

class XmlOptionFile
{
public:
  XmlOptionFile(QString filename, QDomDocument def);
  XmlOptionFile();

  StringOptionItem getStringOption(const QString& uid);
  QDomElement getElement();
  QDomElement getElement(QString level1);
  QDomElement getElement(QString level1, QString level2);
  void save();

private:
  void load();
  QDomElement safeGetElement(QDomElement parent, QString childName);

  QDomDocument mDocument;
  QString mFilename;
};


} // namespace ssc

#endif /* SSCXMLOPTIONITEM_H_ */
