/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxXMLNodeWrapper.h"

#include <QDomNode>
#include <QStringList>
#include <iostream>

namespace cx
{

XMLNodeAdder::XMLNodeAdder(QDomNode node) : mNode(node)
{
}

XMLNodeAdder XMLNodeAdder::descend(QString path)
{
	QStringList pathList = path.split("/");
	QDomElement current = mNode.toElement();

	if (current.isNull())
		return XMLNodeAdder(current);

	for (int i = 0; i < pathList.size(); ++i)
	{
		QDomElement next = current.namedItem(pathList[i]).toElement();

		if (next.isNull())
		{
			next = mNode.ownerDocument().createElement(pathList[i]);
			current.appendChild(next);
		}

		current = next;
	}

	return XMLNodeAdder(current);
}

QDomNode XMLNodeAdder::node()
{
	return mNode;
}

QDomElement XMLNodeAdder::addTextToElement(QString name, QString text)
{
	QDomElement element = this->addElement(name);
	element.appendChild(this->document().createTextNode(text));
	return element;
}

QDomElement XMLNodeAdder::addElement(QString name)
{
	QDomElement element = this->document().createElement(name);
	mNode.appendChild(element);
	return element;
}

QDomDocument XMLNodeAdder::document()
{
	return mNode.ownerDocument();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

XMLNodeParser::XMLNodeParser(QDomNode node) : mNode(node)
{
}

XMLNodeParser XMLNodeParser::descend(QString path)
{
	QStringList pathList = path.split("/");
	QDomElement current = mNode.toElement();

	if (current.isNull())
		return XMLNodeParser(current);

	for (int i = 0; i < pathList.size(); ++i)
	{
		QDomElement next = current.namedItem(pathList[i]).toElement();

		if (next.isNull())
			return XMLNodeParser(QDomNode());

		current = next;
	}

	return XMLNodeParser(current);
}

QString XMLNodeParser::parseTextFromElement(QString name)
{
	return mNode.namedItem(name).toElement().text();
}

double XMLNodeParser::parseDoubleFromElementWithDefault(QString name, double defaultValue)
{
	QString text = mNode.namedItem(name).toElement().text();
	bool ok;
	double val = text.toDouble(&ok);
	if (ok)
		return val;
	return defaultValue;
}

QStringList XMLNodeParser::parseTextFromDuplicateElements(QString name)
{
	QStringList retval;
	for (QDomElement elem = mNode.firstChildElement(name);
		 !elem.isNull();
		 elem = elem.nextSiblingElement(name))
	{
		retval << elem.text();
	}
	return retval;
}

std::vector<QDomElement> XMLNodeParser::getDuplicateElements(QString name)
{
	std::vector<QDomElement> retval;
	for (QDomElement elem = mNode.firstChildElement(name);
		 !elem.isNull();
		 elem = elem.nextSiblingElement(name))
	{
		retval.push_back(elem);
	}
	return retval;
}


QDomElement XMLNodeParser::parseElement(QString name)
{
	return mNode.namedItem(name).toElement();
}

QDomNode XMLNodeParser::node()
{
	return mNode;
}


} // namespace cx
