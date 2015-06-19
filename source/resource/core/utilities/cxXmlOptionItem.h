/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


/*
 * sscXmlOptionItem.h
 *
 *  Created on: May 28, 2010
 *      Author: christiana
 */
#ifndef CXXMLOPTIONITEM_H_
#define CXXMLOPTIONITEM_H_

#include "cxResourceExport.h"

#include <QString>
#include <QDomElement>
#include <QVariant>

namespace cx
{

/**\brief Helper class for storing one string value in an xml document.
 *
 * Init with the parent (root) node and an uid,
 * then read/write the value.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT XmlOptionItem
{
public:
	XmlOptionItem()
	{
	}
	XmlOptionItem(const QString& uid, QDomElement root);
	QString readValue(const QString& defval) const;
	void writeValue(const QString& val);

	QVariant readVariant(const QVariant& defval=QVariant()) const;
	void writeVariant(const QVariant& val);

private:
	QDomElement findElemFromUid(const QString& uid, QDomNode root) const;
	QString mUid;
	QDomElement mRoot;

	static QString SerializeDataToB64String(const QVariant& data);
	static QVariant DeserializeB64String(const QString& serializedVariant);
};

/**\brief Helper class for xml files used to store ssc/cx data.
 *
 * Both the xml document and filename are stored,
 * and save/load are provided.
 *
 * Several instances of the same file points to the same internal
 * object, in the same way as QSettings. This means that they share the
 * QDomDocument, and is NOT threadsafe.
 *
 * The object has a current element, which is used to ease work
 * at a specified level in the hierarchy. Use descend/ascend
 * to change the current element.
 *
 * The getElement() methods are used to retrieve QDomElements relative
 * to the current element.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT XmlOptionFile
{
public:
	static XmlOptionFile createNull(); ///< create an empty document

	explicit XmlOptionFile(QString filename); ///< create from filename, create trivial document of type name and root node if no file exists.
	XmlOptionFile();
	~XmlOptionFile();

	QString getFileName();

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
    static void printDocument(QDomDocument document);
    static void printElement(QDomElement element);

private:
	void load();

	QString mFilename;
	QDomDocument mDocument;
	QDomElement mCurrentElement; ///< all getElement() operations are performed relative to this node.

};

} // namespace cx

#endif /* CXXMLOPTIONITEM_H_ */
