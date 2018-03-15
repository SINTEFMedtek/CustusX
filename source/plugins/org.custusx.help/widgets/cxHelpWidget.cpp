/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxHelpIndexWidget.h"
#include "cxSettings.h"
#include "cxLogger.h"
#include "cxDataLocations.h"
#include <QDesktopServices>

namespace cx
{

HelpWidget::HelpWidget(HelpEnginePtr engine, QWidget* parent) :
	BaseWidget(parent, "help_widget", "Help"),
	mVerticalLayout(NULL),
	mTabWidget(NULL),
	mEngine(engine)
{
}

void HelpWidget::setup()
{
	if (mVerticalLayout)
		return;

//	this->setToolTip("Context-sensitive and browser help");
	mVerticalLayout = new QVBoxLayout(this);
	mVerticalLayout->setMargin(0);
	mVerticalLayout->setSpacing(0);
	this->setLayout(mVerticalLayout);
	mTabWidget = new QTabWidget(this);
	mTabWidget->setElideMode(Qt::ElideRight);

	QSplitter *splitter = new QSplitter(Qt::Horizontal);
	mSplitter = splitter;

	HelpBrowser *browser = new HelpBrowser(this, mEngine);
	browser->listenToEngineKeywordActivated();
	connect(this, &HelpWidget::requestShowLink,
			browser, &HelpBrowser::setSource);
	mBrowser = browser;

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	//	buttonLayout->setMargin(0);
	mVerticalLayout->addLayout(buttonLayout);

	splitter->insertWidget(0, mTabWidget);
	splitter->insertWidget(1, browser);
	splitter->setStretchFactor(1, 1);
	mVerticalLayout->addWidget(splitter);

	this->addContentWidget(mTabWidget, buttonLayout);
	this->addSearchWidget(mTabWidget, buttonLayout);
	this->addIndexWidget(mTabWidget, buttonLayout);

	this->addToggleTabWidgetButton(buttonLayout);
	this->addWebNavigationButtons(buttonLayout);
	this->addWebButton(buttonLayout);
	buttonLayout->addStretch();

	browser->showHelpForKeyword("user_doc_overview");

	bool navVis = settings()->value("org.custusx.help/navigationVisible").toBool();
	mTabWidget->setVisible(navVis);
}

HelpWidget::~HelpWidget()
{}

QSize HelpWidget::sizeHint() const
{
	// Removing this gives a very small initial size.
	// This also seems to be the widest possible.
	return QSize(500,30);
}


void HelpWidget::addContentWidget(QTabWidget* tabWidget, QBoxLayout* buttonLayout)
{
	QHelpContentWidget* contentWidget = mEngine->engine()->contentWidget();
	tabWidget->addTab(contentWidget, "contents");

	boost::function<void()> f = boost::bind(&QHelpContentWidget::expandToDepth, contentWidget, 2);
	connect(mEngine->engine()->contentModel(), &QHelpContentModel::contentsCreated, f);
	contentWidget->expandToDepth(2); // in case contents have been created

	connect(mEngine->engine()->contentWidget(), &QHelpContentWidget::linkActivated,
			this, &HelpWidget::requestShowLink);
}

void HelpWidget::addWebNavigationButtons(QBoxLayout* buttonLayout)
{
	QAction* back = this->createAction(this,
									   QIcon(":/icons/open_icon_library/arrow-left-3.png"),
									   "Back", "Back to previous page",
									   SLOT(backSlot()),
									   buttonLayout, new CXSmallToolButton());

	QAction* forward = this->createAction(this,
										  QIcon(":/icons/open_icon_library/arrow-right-3.png"),
										  "Forward", "Forward to next page",
										  SLOT(forwardSlot()),
										  buttonLayout, new CXSmallToolButton());

	connect(mBrowser, SIGNAL(backwardAvailable(bool)), back, SLOT(setEnabled(bool)));
	connect(mBrowser, SIGNAL(forwardAvailable(bool)), forward, SLOT(setEnabled(bool)));
}

void HelpWidget::addWebButton(QBoxLayout* buttonLayout)
{
	this->createAction2(this,
					   QIcon(":/icons/open_icon_library/applications-internet.png"),
					   "Web", "Open Web Documentation",
					   &HelpWidget::onGotoDocumentation,
					   buttonLayout, new CXSmallToolButton());
}

void HelpWidget::onGotoDocumentation()
{
	QString url = DataLocations::getWebsiteUserDocumentationURL();
	QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}

void HelpWidget::backSlot()
{
	mBrowser->backward();
}

void HelpWidget::forwardSlot()
{
	mBrowser->forward();
}

void HelpWidget::addToggleTabWidgetButton(QBoxLayout* buttonLayout)
{
	QAction* action = this->createAction(this,
										   QIcon(":/icons/open_icon_library/view-list-tree.png"),
										   "Toggle show navigation controls", "",
										   SLOT(toggleShowNavigationControls()),
										   NULL);
	action->setCheckable(true);
	CXSmallToolButton* button = new CXSmallToolButton();
	button->setDefaultAction(action);
	buttonLayout->addWidget(button);
	mShowNavigationControlsAction = action;
}

void HelpWidget::addIndexWidget(QTabWidget* tabWidget, QBoxLayout* buttonLayout)
{
	mIndexWidget = new HelpIndexWidget(mEngine, this);
	tabWidget->addTab(mIndexWidget, "index");

	connect(mIndexWidget, &HelpIndexWidget::requestShowLink,
			this, &HelpWidget::requestShowLink);
}

void HelpWidget::addSearchWidget(QTabWidget* tabWidget, QBoxLayout* buttonLayout)
{
	mSearchWidget = new HelpSearchWidget(mEngine, this);
	tabWidget->addTab(mSearchWidget, "search");
	connect(mSearchWidget, &HelpSearchWidget::requestShowLink,
			this, &HelpWidget::requestShowLink);
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
	this->setup();
}

void HelpWidget::toggleShowNavigationControls()
{
	if (mTabWidget->isVisible())
		mTabWidget->hide();
	else
	{
		mTabWidget->show();

		QList<int> sizes = mSplitter->sizes();
		if (sizes[0]==0)
		{
			sizes[0] = sizes[1]*1/3;
			sizes[1] = sizes[1]*2/3;
			mSplitter->setSizes(sizes);
		}

	}
	settings()->setValue("org.custusx.help/navigationVisible", mTabWidget->isVisible());
}

}//end namespace cx
