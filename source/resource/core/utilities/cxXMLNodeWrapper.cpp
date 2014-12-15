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
