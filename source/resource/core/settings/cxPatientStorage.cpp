/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPatientStorage.h"
#include "cxSessionStorageService.h"
#include "cxXMLNodeWrapper.h"

namespace cx
{

PatientStorage::PatientStorage(SessionStorageServicePtr sessionStorageService, QString baseNodeName, bool delayedLoad) :
	mBaseNodeName(baseNodeName)
{
	if(delayedLoad)
		connect(sessionStorageService.get(), &SessionStorageService::isLoadingSecond, this, &PatientStorage::duringLoadPatientSlot);
	connect(sessionStorageService.get(), &SessionStorageService::isLoading, this, &PatientStorage::duringLoadPatientSlot);
	connect(sessionStorageService.get(), &SessionStorageService::isSaving, this, &PatientStorage::duringSavePatientSlot);
}

void PatientStorage::storeVariable(QString nodeName, boost::function<QString ()> getValueFunction, boost::function<void (QString)> setValueFunction)
{
	mGetFunctions[nodeName] = getValueFunction;
	mSetFunctions[nodeName] = setValueFunction;
}


void PatientStorage::duringSavePatientSlot(QDomElement& node)
{
	XMLNodeAdder root(node);
	QDomElement baseNode = root.descend(mBaseNodeName).node().toElement();
	this->addXml(baseNode);
}

void PatientStorage::duringLoadPatientSlot(QDomElement& node)
{
	XMLNodeParser root(node);
	QDomElement baseNode = root.descend(mBaseNodeName).node().toElement();
	if (!baseNode.isNull())
		this->parseXml(baseNode);
}

void PatientStorage::addXml(QDomNode& parentNode)
{
	QDomDocument doc = parentNode.ownerDocument();

	std::map<QString, boost::function<QString()> >::iterator iter;
	for(iter = mGetFunctions.begin(); iter != mGetFunctions.end(); ++iter)
	{
		QDomElement newDataNode = doc.createElement(iter->first);
		newDataNode.appendChild(doc.createTextNode(iter->second()));
		parentNode.appendChild(newDataNode);
	}
}

void PatientStorage::parseXml(QDomNode& dataNode)
{
	std::map<QString, boost::function<void(QString value)> >::iterator iter;
	for(iter = mSetFunctions.begin(); iter != mSetFunctions.end(); ++iter)
	{
		QString nodeValue = dataNode.namedItem(iter->first).toElement().text();
		iter->second(nodeValue);
	}
}

} //cx