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

#include "boost/bind.hpp"
#include "boost/function.hpp"
#include <QHelpEngine>
#include <QSplitter>
#include <QHelpContentWidget>
#include <QHelpIndexWidget>
#include <QTabWidget>

#include "cxTypeConversions.h"
#include "cxHelpEngine.h"
#include "cxHelpBrowser.h"
#include "cxHelpSearchWidget.h"

namespace cx
{

HelpWidget::HelpWidget(HelpEnginePtr engine, QWidget* parent) :
	BaseWidget(parent, "HelpWidget", "Help"),
	mVerticalLayout(new QVBoxLayout(this)),
	mTabWidget(new QTabWidget(this))
{
	this->setLayout(mVerticalLayout);
	mTabWidget->setElideMode(Qt::ElideRight);

	mEngine = engine;

	mSearchWidget = new HelpSearchWidget(mEngine, this);

	QSplitter *helpPanel = new QSplitter(Qt::Horizontal);
	HelpBrowser *helpBrowser = new HelpBrowser(this, mEngine);
	QHelpContentWidget* contentWidget = mEngine->engine()->contentWidget();

//	mShowSearchAction = this->createAction(this,
//					QIcon(":/icons/open_icon_library/eye.png.png"),
//					"Toggle show search help", "",
//					SLOT(toggleShowSearchHelp()),
//					NULL);
//	mShowSearchAction->setCheckable(true);
//	CXSmallToolButton* toggleShowSearchButton = new CXSmallToolButton();
//	toggleShowSearchButton->setDefaultAction(mShowSearchAction);

	helpPanel->insertWidget(0, mTabWidget);
	helpPanel->insertWidget(1, helpBrowser);
	helpPanel->setStretchFactor(1, 1);
	mVerticalLayout->addWidget(helpPanel);

	mTabWidget->addTab(contentWidget, "contents");
	mTabWidget->addTab(mSearchWidget, "search");
	mTabWidget->addTab(mEngine->engine()->indexWidget(), "index");

	boost::function<void()> f = boost::bind(&QHelpContentWidget::expandToDepth, contentWidget, 2);
	connect(mEngine->engine()->contentModel(), &QHelpContentModel::contentsCreated, f);

	connect(mEngine->engine()->contentWidget(), &QHelpContentWidget::linkActivated,
			helpBrowser, &HelpBrowser::setSource);
	connect(mSearchWidget, &HelpSearchWidget::requestShowLink,
			helpBrowser, &HelpBrowser::setSource);

	helpBrowser->showHelpForKeyword("mainpage_overview");

	//  //layout
	//  mVerticalLayout->addLayout(buttonLayout);
	//  mVerticalLayout->addWidget(mTable, 1);
	//  mVerticalLayout->addWidget(mEditWidgets, 0);
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

void HelpWidget::toggleShowSearchHelp()
{
	if (mSearchWidget->isVisible())
		mSearchWidget->hide();
	else
		mSearchWidget->show();
}

}//end namespace cx
