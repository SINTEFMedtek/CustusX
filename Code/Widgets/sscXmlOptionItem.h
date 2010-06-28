/*
 * sscXmlOptionItem.h
 *
 *  Created on: May 28, 2010
 *      Author: christiana
 */
#ifndef SSCXMLOPTIONITEM_H_
#define SSCXMLOPTIONITEM_H_

#include <QString>
#include <QDomElement>

namespace ssc
{

/**Helper class for storing one string value in an xml document.
 *
 * Init with the parent (root) node and an uid,
 * then read/write the value.
 *
 */
class XmlOptionItem
{
public:
	XmlOptionItem() {}
	XmlOptionItem(const QString& uid,
		      QDomElement root);
	QString readValue(const QString& defval) const;
	void writeValue(const QString& val);
private:
	QDomElement findElemFromUid(const QString& uid, QDomNode root) const;
	QDomElement mRoot;
	QString mUid;
};


class XmlOptionFile
{
public:
  XmlOptionFile(QString filename, QDomDocument def);
  XmlOptionFile();

  //StringOptionItem getStringOption(const QString& uid);
  QDomElement getElement();
  QDomElement getElement(QString level1);
  QDomElement getElement(QString level1, QString level2);
  void save();

private:
  void load();
  QDomElement safeGetElement(QDomElement parent, QString childName);
  
  QString mFilename;
  QDomDocument mDocument;
};


} // namespace ssc

#endif /* SSCXMLOPTIONITEM_H_ */
