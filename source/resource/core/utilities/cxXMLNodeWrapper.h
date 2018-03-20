/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
