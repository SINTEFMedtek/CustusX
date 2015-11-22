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
#include "cxBrowserWidget.h"

#include <QTreeView>
#include "cxTreeItemModel.h"
#include "cxLogger.h"

namespace cx
{

BrowserWidget::BrowserWidget(QWidget* parent, VisServicesPtr services) :
	BaseWidget(parent, "BrowserWidget", "Browser"),
	mVerticalLayout(new QVBoxLayout(this)),
	mServices(services)
{
	this->setModified();
	mModel = new TreeItemModel(services, this);
	connect(mModel, SIGNAL(hasBeenReset()), this, SLOT(setModified()));
	//  mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mModel->getFilter()));

	//layout
	mTreeView = new QTreeView(this);
	mTreeView->setRootIsDecorated(false);
	mTreeView->setModel(mModel);
	mModel->setSelectionModel(mTreeView->selectionModel());
	mVerticalLayout->addWidget(mTreeView);

}

BrowserWidget::~BrowserWidget()
{
}


void BrowserWidget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	//  connect(patientService().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(populateTreeWidget()));
	//  connect(viewService().get(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)), this, SLOT(populateTreeWidget()));
	//  connect(viewService().get(), SIGNAL(activeLayoutChanged()), this, SLOT(populateTreeWidget()));
	//  connect(viewService().get(), SIGNAL(activeViewChanged()), this, SLOT(populateTreeWidget()));

	//  this->populateTreeWidget();
}
void BrowserWidget::closeEvent(QCloseEvent* event)
{
	QWidget::closeEvent(event);
	//  disconnect(patientService().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(populateTreeWidget()));
	//  disconnect(viewService().get(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)), this, SLOT(populateTreeWidget()));
	//  disconnect(viewService().get(), SIGNAL(activeLayoutChanged()), this, SLOT(populateTreeWidget()));
	//  disconnect(viewService().get(), SIGNAL(activeViewChanged()), this, SLOT(populateTreeWidget()));
}

void BrowserWidget::prePaintEvent()
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "BrowserWidget::prePaintEvent()";
	mModel->update();
	this->resetView();
}

void BrowserWidget::resetView()
{
	mTreeView->expandToDepth(4);
	for (unsigned i=0; i<4; ++i)
		mTreeView->resizeColumnToContents(i);
}


}//end namespace cx
