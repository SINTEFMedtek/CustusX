// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXXMLNODEWRAPPER_H
#define CXXMLNODEWRAPPER_H

#include <QDomNode>
#include <QStringList>


namespace cx
{

/** Helper for CustusX addXml/parseXml framework
 *
 * \ingroup cx
 * \date 2014-02-27
 * \author christiana
 */
class XMLNodeAdder
{
public:
	XMLNodeAdder(QDomNode node);

	void addTextToElement(QString name, QString text);

	template<class T>
	void addObjectToElement(QString name, T object)
	{
		QDomElement element = this->addElement(name);
		object->addXml(element);
	}

	QDomElement addElement(QString name);

private:
	QDomNode mNode;
	QDomDocument document();
};

/** Helper for CustusX addXml/parseXml framework
 *
 * \ingroup cx
 * \date 2014-02-27
 * \author christiana
 */
class XMLNodeParser
{
public:
	XMLNodeParser(QDomNode node);

	QString parseTextFromElement(QString name);
	double parseDoubleFromElementWithDefault(QString name, double defaultValue);
	QStringList parseTextFromDuplicateElements(QString name);

	template<class T>
	void parseObjectFromElement(QString name, T object)
	{
		object->parseXml(mNode.namedItem(name));
	}

	QDomElement parseElement(QString name);

private:
	QDomNode mNode;
};

} // namespace cx


#endif // CXXMLNODEWRAPPER_H
