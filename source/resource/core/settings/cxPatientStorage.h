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

#ifndef CXPATIENTSTORAGE_H
#define CXPATIENTSTORAGE_H

#include "cxResourceExport.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

class QDomElement;
class QDomNode;

namespace cx
{
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;

/**\brief Helper class for storing variables in the patient file.
 *
 * Use boost::bind to supply get/set functions.
 *
 * E.g.:
 * storeVariable("variableName", boost::bind(&VariableToBeStoredClass::getValue, &variableToBeStored), boost::bind(&VariableToBeStoredClass::setValue, &variableToBeStored, _1));
 *
 * or:
 * storeVariable("variableName", boost::bind(&VariableToBeStoredClass::getValue, variableToBeStoredSmartPtr), boost::bind(&VariableToBeStoredClass::setValue, variableToBeStoredSmartPtr, _1));
 *
 *  \date Sep 07, 2015
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT PatientStorage : public QObject
{
	Q_OBJECT
public:
	PatientStorage(SessionStorageServicePtr sessionStorageService, QString baseNodeName);
	/**
	 * @brief storeVariable Store a variable in the patient file
	 * @param nodeName Name of the node to store
	 * @param getValueFunction Suppy a get function returning a QString. Use boost::bind
	 * @param setValueFunction Suppy a set function with a QString as parameter. Use boost::bind
	 */
	void storeVariable(QString nodeName, boost::function<QString ()> getValueFunction, boost::function<void (QString)> setValueFunction);
private slots:
	void duringSavePatientSlot(QDomElement &node);
	void duringLoadPatientSlot(QDomElement &node);
private:
	QString mBaseNodeName;
	std::map<QString, boost::function<QString()> > mGetFunctions;
	std::map<QString, boost::function<void(QString value)> > mSetFunctions;
	void addXml(QDomNode &parentNode);
	void parseXml(QDomNode &dataNode);
};

} //cx
#endif // CXPATIENTSTORAGE_H
