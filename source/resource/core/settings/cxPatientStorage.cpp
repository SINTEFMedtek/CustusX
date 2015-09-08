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

#include "cxPatientStorage.h"
#include "cxSessionStorageService.h"
#include "cxXMLNodeWrapper.h"

namespace cx
{

PatientStorage::PatientStorage(SessionStorageServicePtr sessionStorageService, QString baseNodeName) :
	mBaseNodeName(baseNodeName)
{
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
	QDomElement m2USRegWidget = root.descend(mBaseNodeName).node().toElement();
	if (!m2USRegWidget.isNull())
		this->parseXml(m2USRegWidget);
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