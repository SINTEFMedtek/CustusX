/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTabbedWidget.h"

#include <iostream>
#include <QTabWidget>
#include <QVBoxLayout>

namespace cx
{
//------------------------------------------------------------------------------
TabbedWidget::TabbedWidget(QWidget* parent, QString objectName, QString windowTitle) :
	BaseWidget(parent, objectName, windowTitle),
	mTabWidget(new QTabWidget(this))
{
	mTabWidget->setElideMode(Qt::ElideRight);
//	mTabWidget->setFocusPolicy(Qt::StrongFocus);
	mLayout = new QVBoxLayout(this);
	mLayout->addWidget(mTabWidget);
	mLayout->setMargin(0); // lots of tabbed widgets in layers use up the desktop. Must reduce.

	connect(mTabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(const int &)));
}

TabbedWidget::~TabbedWidget()
{}

void TabbedWidget::addTab(BaseWidget* newTab, QString newTabName)
{
  int index = mTabWidget->addTab(newTab, newTabName);
  mTabWidget->setTabToolTip(index, newTab->toolTip());
  mTabWidget->setTabWhatsThis(index, newTab->whatsThis());
}

void TabbedWidget::insertWidgetAtTop(QWidget* newWidget)
{
	mLayout->insertWidget(0, newWidget);
}

void TabbedWidget::tabChanged(const int &index)
{
	// A new tab has been selected, so give the focus to its widget
	QWidget* widget = mTabWidget->widget(index);
	if (widget)
		widget->setFocus();
}

void TabbedWidget::setDefaultWidget(QWidget *widget)
{
    mTabWidget->setCurrentWidget(widget);
}


//------------------------------------------------------------------------------
} //namespace cx
