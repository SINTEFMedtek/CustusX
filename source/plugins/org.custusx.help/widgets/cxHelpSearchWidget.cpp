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
	std::cout << "start indexing: "  << std::endl;
}

void HelpSearchWidget::indexingFinished()
{
	std::cout << "stop indexing: "  << std::endl;
}

void HelpSearchWidget::searchingIsStarted()
{
	std::cout << "started search: "  << std::endl;
}
void HelpSearchWidget::searchingIsFinished(int i)
{
	std::cout << "finished search: " << i << std::endl;
}

}//end namespace cx
