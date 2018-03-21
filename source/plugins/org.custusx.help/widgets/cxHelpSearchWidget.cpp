/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxHelpSearchWidget.h"

#include <QVBoxLayout>
#include <QHelpEngine>
#include <QHelpSearchEngine>
#include <QHelpSearchQueryWidget>
#include <QHelpSearchResultWidget>
#include <iostream>
#include "cxTypeConversions.h"
#include "cxHelpEngine.h"

namespace cx
{

HelpSearchWidget::HelpSearchWidget(HelpEnginePtr engine, QWidget* parent) :
	QWidget(parent),
	mVerticalLayout(new QVBoxLayout(this)),
	mEngine(engine)
{
	this->setLayout(mVerticalLayout);
	mVerticalLayout->setMargin(0);

	helpSearchEngine = new QHelpSearchEngine(mEngine->engine());
	connect(helpSearchEngine, SIGNAL(indexingStarted()), this, SLOT(indexingStarted()));
	connect(helpSearchEngine, SIGNAL(indexingFinished()), this, SLOT(indexingFinished()));
	connect(helpSearchEngine, SIGNAL(searchingStarted()), this, SLOT(searchingIsStarted()));
	connect(helpSearchEngine, SIGNAL(searchingFinished(int)), this, SLOT(searchingIsFinished(int)));

	mVerticalLayout->addWidget(helpSearchEngine->queryWidget());
	mVerticalLayout->addWidget(helpSearchEngine->resultWidget());

	connect(helpSearchEngine->queryWidget(), SIGNAL(search()), this, SLOT(search()));

	helpSearchEngine->reindexDocumentation();

	connect(helpSearchEngine->resultWidget(),
			SIGNAL(requestShowLink(const QUrl&)),
			this, SIGNAL(requestShowLink(const QUrl&)));
}

void HelpSearchWidget::search()
{
	QList<QHelpSearchQuery> query = helpSearchEngine->queryWidget()->query();
	helpSearchEngine->search(query);
}

void HelpSearchWidget::indexingStarted()
{
//	std::cout << "start indexing: "  << std::endl;
}

void HelpSearchWidget::indexingFinished()
{
//	std::cout << "stop indexing: "  << std::endl;
}

void HelpSearchWidget::searchingIsStarted()
{
//	std::cout << "started search: "  << std::endl;
}
void HelpSearchWidget::searchingIsFinished(int i)
{
//	std::cout << "finished search: " << i << std::endl;
}

}//end namespace cx
