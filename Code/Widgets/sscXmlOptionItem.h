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

  QDomDocument mDocument;
  QString mFilename;
};


} // namespace ssc

#endif /* SSCXMLOPTIONITEM_H_ */
