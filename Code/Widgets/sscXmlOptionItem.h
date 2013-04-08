// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

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

/**\brief Helper class for storing one string value in an xml document.
 *
 * Init with the parent (root) node and an uid,
 * then read/write the value.
 *
 * \ingroup sscWidget
 */
class XmlOptionItem
{
public:
	XmlOptionItem()
	{
	}
	XmlOptionItem(const QString& uid, QDomElement root);
	QString readValue(const QString& defval) const;
	void writeValue(const QString& val);
private:
	QDomElement findElemFromUid(const QString& uid, QDomNode root) const;
	QString mUid;
	QDomElement mRoot;
};

/**\brief Helper class for xml files used to store ssc/cx data.
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
 *
 * \ingroup sscWidget
 */
class XmlOptionFile
{
public:
	static XmlOptionFile createNull(); ///< create an empty document

	explicit XmlOptionFile(QString filename, QString name = ""); ///< create from filename, create trivial document of type name and root node if no file exists.
	XmlOptionFile();
	~XmlOptionFile();

	bool isNull() const; ///< checks if this is null

	XmlOptionFile root() const; ///< set the current element to root
	XmlOptionFile descend(QString element) const; ///< step one level down in the xml tree
	XmlOptionFile descend(QString element, QString attributeName, QString attributeValue) const; // stepdown to element with given attribute
	XmlOptionFile ascend() const; ///< step one level up in the xml tree
	XmlOptionFile tryDescend(QString element, QString attributeName, QString attributeValue) const; // stepdown only if present

	QDomDocument getDocument(); ///< returns the document
	QDomElement getElement(); ///< return the current element
	QDomElement getElement(QString level1); ///< return a element below the current element. Guaranteed to exist.
	QDomElement getElement(QString level1, QString level2); ///< return a element two levels below the current element. Guaranteed to exist.

	void save(); ///< save entire document.

	void removeChildren(); ///< remove all child nodes of the current element.
	void deleteNode(); ///< Delete the current node

	QDomElement safeGetElement(QDomElement parent, QString childName);

	//Debugging
	void printDocument(); ///< print the entire document
	void printElement(); ///< print just the current element

private:
	void load();

	QString mFilename;
	QDomDocument mDocument;
	QDomElement mCurrentElement; ///< all getElement() operations are performed relative to this node.

	static class SharedDocuments* mSharedDocuments;
};

} // namespace ssc

#endif /* SSCXMLOPTIONITEM_H_ */
