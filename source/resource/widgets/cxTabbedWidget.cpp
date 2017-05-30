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
