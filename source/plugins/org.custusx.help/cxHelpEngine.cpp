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
#include "cxLogger.h"
#include "cxProfile.h"

namespace cx
{

HelpEngine::HelpEngine()
{
	QString helpFile = profile()->getPath() + "/cx_user_doc.qhc";
	helpEngine = new QHelpEngine(helpFile, NULL);
	bool success = helpEngine->setupData();
	//		CX_LOG_CHANNEL_DEBUG("CA") << "Regdocs loaded: " << helpEngine->registeredDocumentations().join("--");

	QString docFile = DataLocations::getDocPath()+"/cx_user_doc.qch";
	helpEngine->registerDocumentation(docFile);
	//		CX_LOG_CHANNEL_DEBUG("CA") << "Setup help data: " << success << " - " << docFile;
	//		CX_LOG_CHANNEL_DEBUG("CA") << "Last help error: " << helpEngine->error();
	//		CX_LOG_CHANNEL_DEBUG("CA") << "Regdocs: " << helpEngine->registeredDocumentations().join("--");

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
//		std::cout << "HelpEngine::focusObjectChanged " << newFocus->objectName() << " -- " << dynamic_cast<QWidget*>(newFocus)->windowTitle() << std::endl;
	QString keyword = this->findBestMatchingKeyword(newFocus);
	if (!keyword.isEmpty())
	{
//				std::cout << "******** keyword: " << keyword << std::endl;
		emit keywordActivated(keyword);
	}
}

bool HelpEngine::isBreakChar(QChar c) const
{
	return c.isDigit() || c.isUpper();
}

bool HelpEngine::isBreakChar(QString text, int index) const
{
	if (!this->isBreakChar(text[index]))
		return false;

	bool prev = true;
	if (index>0)
		prev = this->isBreakChar(text[index-1]);

	bool next = true;
	if (index+1<text.size())
		next = this->isBreakChar(text[index+1]);

	if (!prev || !next)
		return true;

	return false;
}

QString HelpEngine::convertToKeyword(QString id) const
{
	// convert camel case strings into whitespace-separated lowercase strings:
	// MyWidget -> my widget
	// myFancy3DWidget2D -> my Fancy 3D Widget 2D -> my fancy 3d widget 2d
	// myFancyDWidget -> my Fancy D Widget -> my fancy D widget
	QString retval;
	retval.push_back(id[0]);
	for (int i=1; i<id.size(); ++i)
	{
		// break condition Q is (uppercase or digit)
		// insert whitespace before Q
		// ignore if previous was Q
		if (this->isBreakChar(id, i))
		{
			retval.push_back("_"); // cant't get doxygenerated anchors to work properly with whitespace
		}
		retval.push_back(id[i]);
	}
	return retval.toLower();
}

QString HelpEngine::findBestMatchingKeyword(QObject* object)
{
	while (object)
	{
		QString id = this->convertToKeyword(object->objectName());
//				std::cout << "    examining " << object->objectName() << ", keyword = " << id << std::endl;

		if (id.contains("help_widget"))
			return "";

		QMap<QString, QUrl> links = this->engine()->linksForIdentifier(id);
		if (!links.empty())
		{
			return id;
		}

		object = object->parent();
	}
	return "";
}

}
