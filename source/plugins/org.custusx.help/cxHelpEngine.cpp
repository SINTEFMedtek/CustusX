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

#include "cxHelpEngine.h"

#include <QHelpEngine>
#include "cxDataLocations.h"
#include <iostream>
#include "cxTypeConversions.h"
#include <QFileInfo>
#include <QApplication>
#include <QWidget>

namespace cx
{

HelpEngine::HelpEngine()
{
	//	helpEngine = new QHelpEngine(DataLocations::getDocPath()+"/wateringmachine.qhc", this);
		QString helpFile = DataLocations::getDocPath()+"/cx_user_doc.qhc";
	//	QString helpFile = DataLocations::getDocPath()+"/doxygen/cx_user_doc.qch"; // virker med QtAssistant
	//	QString helpFile;
//		std::cout << "helpFile " << helpFile << " -- " << QFileInfo(helpFile).exists() << std::endl;
		helpEngine = new QHelpEngine(helpFile, NULL);
		helpEngine->setupData();

		connect(qApp, SIGNAL(focusObjectChanged(QObject*)), this, SLOT(focusObjectChanged(QObject*)));
		connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
}

void HelpEngine::focusChanged(QWidget * old, QWidget * now)
{
	if (!now)
		return;
//	std::cout << "new widget focus: " << now->objectName() << std::endl;

}

void HelpEngine::focusObjectChanged(QObject* newFocus)
{
	if (!newFocus)
		return;
//	std::cout << "HelpEngine::focusObjectChanged " << newFocus->objectName() << " -- " << dynamic_cast<QWidget*>(newFocus)->windowTitle() << std::endl;
//	newFocus
	QString keyword = this->findBestMatchingKeyword(newFocus);
	if (!keyword.isEmpty())
	{
//		std::cout << "******** keyword: " << keyword << std::endl;
		emit keywordActivated(keyword);
	}
}

QString HelpEngine::findBestMatchingKeyword(QObject* object)
{
	while (object)
	{
//		std::cout << "    examining " << object->objectName() << std::endl;
		if (mKeywords.count(object))
			return mKeywords[object];
		object = object->parent();
	}
	return "";
}

void HelpEngine::registerWidget(QWidget* widget, QString keyword)
{
	mKeywords[widget] = keyword;
}

}
