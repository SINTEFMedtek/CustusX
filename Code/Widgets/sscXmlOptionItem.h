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
	QString mUid;
	QDomElement mRoot;
};

/**Helper class for xml files used to store ssc/cx data.
 *
 * Both the xml document and filename are stored,
 * and save/load are provided.
 *
 * Several instances of the same object points to the same internal
 * object, in the same way as QSettings.
 *
 * The object has a current element, which is used to ease work
 * at a specified level in the hierarchy. Use descend/ascend
 * to change the current element.
 *
 * The getElement() methods are used to retrieve QDomElements relative
 * to the current element.
 */
class XmlOptionFile
{
public:
  explicit XmlOptionFile(QString filename, QString name=""); ///< create from filename, create trivial document of type name and root node if no file exists.
  XmlOptionFile();
  ~XmlOptionFile();

  XmlOptionFile descend(QString element) const; ///< step one level down in the xml tree
  XmlOptionFile ascend() const; ///< step one level up in the xml tree

  QDomDocument getDocument() { return mDocument; }
  QDomElement getElement(); ///< return the current element
  QDomElement getElement(QString level1); ///< return a element below the current element. Guaranteed to exist.
  QDomElement getElement(QString level1, QString level2);; ///< return a element two levels below the current element. Guaranteed to exist.
  void save(); ///< save entire document.

  void removeChildren(); ///< remove all child nodes of the current element.

private:
  void load();
  QDomElement safeGetElement(QDomElement parent, QString childName);
  
  QString mFilename;
  QDomDocument mDocument;
  QDomElement mCurrentElement; ///< all getElement() operations are performed relative to this node.

  static class SharedDocuments* mSharedDocuments;
};


} // namespace ssc

#endif /* SSCXMLOPTIONITEM_H_ */
