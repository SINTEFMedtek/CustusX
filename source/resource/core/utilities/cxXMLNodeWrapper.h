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
#ifndef CXXMLNODEWRAPPER_H
#define CXXMLNODEWRAPPER_H

#include "cxResourceExport.h"

#include <QDomNode>
#include <QStringList>
#include <vector>

namespace cx
{

/**
 * \file
 * \addtogroup cx_resource_core_utilities
 * @{
 */

/** Helper for CustusX addXml/parseXml framework
 *
 * \date 2014-02-27
 * \author christiana
 */
class cxResource_EXPORT XMLNodeAdder
{
public:
	XMLNodeAdder(QDomNode node);

	/**
	 * Use the /-separated path to descend into the root children recursively.
	 * Create elements if necessary.
	 */
	XMLNodeAdder descend(QString path);

	QDomElement addTextToElement(QString name, QString text);

	template<class T>
	void addObjectToElement(QString name, T object)
	{
		QDomElement element = this->addElement(name);
		object->addXml(element);
	}

	QDomElement addElement(QString name);
	QDomNode node();

private:
	QDomNode mNode;
	QDomDocument document();
};

/** Helper for CustusX addXml/parseXml framework
 *
 * \date 2014-02-27
 * \author christiana
 */
class cxResource_EXPORT XMLNodeParser
{
public:
	XMLNodeParser(QDomNode node);
	/**
	 * Use the /-separated path to descend into the root children recursively.
	 */
	XMLNodeParser descend(QString path);
	QDomNode node();

	QString parseTextFromElement(QString name);
	double parseDoubleFromElementWithDefault(QString name, double defaultValue);
	QStringList parseTextFromDuplicateElements(QString name);
	std::vector<QDomElement> getDuplicateElements(QString name);

	template<class T>
	void parseObjectFromElement(QString name, T object)
	{
		object->parseXml(mNode.namedItem(name));
	}

	QDomElement parseElement(QString name);

private:
	QDomNode mNode;
};

/**
 * @}
 */

} // namespace cx


#endif // CXXMLNODEWRAPPER_H
