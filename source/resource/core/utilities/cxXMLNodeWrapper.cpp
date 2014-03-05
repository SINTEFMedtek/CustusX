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
#include "cxXMLNodeWrapper.h"

#include <QDomNode>
#include <QStringList>

namespace cx
{

XMLNodeAdder::XMLNodeAdder(QDomNode node) : mNode(node)
{
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


} // namespace cx
