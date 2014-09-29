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

#include "cxHelpWidget.h"

#include "cxLogger.h"
#include <QHelpEngine>
#include "cxDataLocations.h"
#include <QSplitter>

#include <QApplication>
#include <QHelpEngineCore>
#include <QHelpContentWidget>
#include <QHelpSearchEngine>
#include <QHelpSearchQueryWidget>
#include <QHelpSearchResultWidget>
#include <QHelpIndexWidget>
#include <iostream>
#include "cxTypeConversions.h"
#include <QFileInfo>
#include "cxHelpEngine.h"

namespace cx
{
HelpBrowser::HelpBrowser(QWidget *parent, HelpEnginePtr engine)
	: QTextBrowser(parent), mEngine(engine)
{
	connect(mEngine.get(), SIGNAL(keywordActivated(QString)), this, SLOT(showHelpForKeyword(const QString&)));
}

void HelpBrowser::showHelpForKeyword(const QString &id)
{
	if (mEngine->engine())
	{
		QMap<QString, QUrl> links = mEngine->engine()->linksForIdentifier(id);
//		std::cout << "[links]" << links.size() << std::endl;
		if (links.count())
		{
			setSource(links.constBegin().value());
		}
	}
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
	QByteArray ba;
	if (type < 4 && mEngine->engine()) {
		QUrl url(name);
		if (name.isRelative())
			url = source().resolved(url);
		ba = mEngine->engine()->fileData(url);
	}
	return ba;
}


///--------------------------------------------------------
///--------------------------------------------------------

///--------------------------------------------------------
///--------------------------------------------------------

HelpWidget::HelpWidget(HelpEnginePtr engine, QWidget* parent) :
  BaseWidget(parent, "HelpWidget", "Help"),
  mVerticalLayout(new QVBoxLayout(this))
{
//	std::cout <<"*************** " <<  engine.get() << std::endl;

  this->setLayout(mVerticalLayout);

	mEngine = engine;

////	helpEngine = new QHelpEngine(DataLocations::getDocPath()+"/wateringmachine.qhc", this);
//	QString helpFile = DataLocations::getDocPath()+"/cx_user_doc.qhc";
////	QString helpFile = DataLocations::getDocPath()+"/doxygen/cx_user_doc.qch"; // virker med QtAssistant
////	QString helpFile;
//	std::cout << "helpFile " << helpFile << " -- " << QFileInfo(helpFile).exists() << std::endl;
//	helpEngine = new QHelpEngine(helpFile, this);
	helpSearchEngine = new QHelpSearchEngine(mEngine->engine());
	connect(helpSearchEngine, SIGNAL(indexingStarted()), this, SLOT(indexingStarted()));
	connect(helpSearchEngine, SIGNAL(indexingFinished()), this, SLOT(indexingFinished()));
	connect(helpSearchEngine, SIGNAL(searchingStarted()), this, SLOT(searchingIsStarted()));
	connect(helpSearchEngine, SIGNAL(searchingFinished(int)), this, SLOT(searchingIsFinished(int)));


	QSplitter *helpPanel = new QSplitter(Qt::Horizontal);
	HelpBrowser *helpBrowser = new HelpBrowser(this, mEngine);

	helpPanel->insertWidget(0, mEngine->engine()->contentWidget());
	helpPanel->insertWidget(1, helpBrowser);
	helpPanel->setStretchFactor(1, 1);
	mVerticalLayout->addWidget(helpPanel);
	mVerticalLayout->addWidget(helpSearchEngine->queryWidget());
	mVerticalLayout->addWidget(helpSearchEngine->resultWidget());
	mVerticalLayout->addWidget(mEngine->engine()->indexWidget());

	connect(helpSearchEngine->queryWidget(), SIGNAL(search()), this, SLOT(search()));

	bool success = mEngine->engine()->setupData();
//	std::cout << "help engine setup success: " << success << std::endl;

	helpSearchEngine->reindexDocumentation();


	connect(mEngine->engine()->contentWidget(),
			SIGNAL(linkActivated(const QUrl &)),
			helpBrowser, SLOT(setSource(const QUrl &)));

	connect(helpSearchEngine->resultWidget(),
			SIGNAL(requestShowLink(const QUrl &)),
			helpBrowser, SLOT(setSource(const QUrl &)));

	helpBrowser->showHelpForKeyword("core_widgets_volume_shading");

//  //layout
//  mVerticalLayout->addLayout(buttonLayout);
//  mVerticalLayout->addWidget(mTable, 1);
//  mVerticalLayout->addWidget(mEditWidgets, 0);
}

void HelpWidget::search()
{
	QList<QHelpSearchQuery> query = helpSearchEngine->queryWidget()->query();
	helpSearchEngine->search(query);
}

void HelpWidget::indexingStarted()
{
	std::cout << "start indexing: "  << std::endl;
}

void HelpWidget::indexingFinished()
{
	std::cout << "stop indexing: "  << std::endl;
}

void HelpWidget::searchingIsStarted()
{
	std::cout << "started search: "  << std::endl;
}
void HelpWidget::searchingIsFinished(int i)
{
	std::cout << "finished search: " << i << std::endl;
}

HelpWidget::~HelpWidget()
{}


QString HelpWidget::defaultWhatsThis() const
{
  return "<html>"
	  "<h3>Experimental help</h3>"
	  "<p></p>"
	  "<p><i></i></p>"
	  "</html>";
}

void HelpWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  this->setModified();
}

void HelpWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
}

void HelpWidget::prePaintEvent()
{
}




}//end namespace cx
