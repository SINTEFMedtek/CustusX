/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
typedef boost::shared_ptr<class PatientStorage> PatientStoragePtr;
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
	PatientStorage(SessionStorageServicePtr sessionStorageService, QString baseNodeName, bool delayedLoad = false);
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
