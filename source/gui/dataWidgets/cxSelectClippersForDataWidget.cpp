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

#include "cxSelectClippersForDataWidget.h"
#include <QTableWidget>
#include <QLabel>
#include <QCheckBox>
#include "cxVisServices.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxClippers.h"
#include "cxLogger.h"
#include "cxInteractiveClipper.h"
#include "cxActiveData.h"
#include "cxSelectDataStringProperty.h"
#include "cxDataSelectWidget.h"

namespace cx
{
SelectClippersForDataWidget::SelectClippersForDataWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "SelectClippersForDataWidget", "Select Clippers"),
	mServices(services),
	mActiveData(services->patient()->getActiveData())
{
	this->initUI();

	ClippersPtr clippers = mServices->view()->getClippers();
	connect(clippers.get(), &Clippers::changed, this, &SelectClippersForDataWidget::updateCheckboxesFromClippers);
	connect(mActiveData.get(), &ActiveData::activeDataChanged, this, &SelectClippersForDataWidget::updateCheckboxesFromClippers);
}

void SelectClippersForDataWidget::initUI()
{
	StringPropertyActiveDataPtr activeDataProperty = StringPropertyActiveData::New(mServices->patient());

	mClipperTableWidget = new QTableWidget(this);

	mHeading = new QLabel("Active clippers");

	mLayout = new QVBoxLayout(this);
	mLayout->addWidget(new DataSelectWidget(mServices->view(), mServices->patient(), this, activeDataProperty));
	mLayout->addWidget(mHeading);
	mLayout->addWidget(mClipperTableWidget);

	this->setupClipperSelectorUI();
	this->createNewCheckBoxesBasedOnClippers();
}

void SelectClippersForDataWidget::updateHeading()
{
	DataPtr activeData = mActiveData->getActive<Data>();
	if (activeData)
		mHeading->setText("Active clippers for: " + activeData->getUid());
	else
		mHeading->setText("Active clippers");
}

void SelectClippersForDataWidget::setupClipperSelectorUI()
{
	ClippersPtr clippers = mServices->view()->getClippers();
	mClipperTableWidget->setColumnCount(2);
	mClipperTableWidget->setRowCount(clippers->size());
	QStringList horizontalHeaders;
	horizontalHeaders << "Clip data" << "Clipper";
	mClipperTableWidget->setHorizontalHeaderLabels(horizontalHeaders);
	mClipperTableWidget->setColumnWidth(2, 300);
}

void SelectClippersForDataWidget::createNewCheckBoxesBasedOnClippers()
{
	mDataCheckBoxes.clear();

	ClippersPtr clippers = mServices->view()->getClippers();
	QStringList clipperNames = clippers->getClipperNames();

	int row = 0;
	for(int i = 0; i < clipperNames.size(); ++i)
	{
		QString clipperName = clipperNames.at(i);
		this->createDataCheckBox(row, clipperName);

		QTableWidgetItem *descriptionItem = new QTableWidgetItem(clipperName);
		mClipperTableWidget->setItem(row++, 1, descriptionItem);
	}
}

void SelectClippersForDataWidget::createDataCheckBox(int row, QString clipperName)
{
	QCheckBox *checkbox = this->createCheckBox(clipperName);
	mClipperTableWidget->setCellWidget(row, 0, checkbox);

	connect(checkbox, &QCheckBox::clicked, this, &SelectClippersForDataWidget::clipDataClicked);
	mDataCheckBoxes[clipperName] = checkbox;
}

QCheckBox *SelectClippersForDataWidget::createCheckBox(QString clipperName)
{
	QCheckBox *checkbox = new QCheckBox();
	checkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	return checkbox;
}

void SelectClippersForDataWidget::updateCheckboxesFromClippers()
{
	this->updateHeading();
	ClippersPtr clippers = mServices->view()->getClippers();
	QStringList clipperNames = clippers->getClipperNames();
	DataPtr activeData = mActiveData->getActive<Data>();

	for(int i = 0; i < clipperNames.size(); ++i)
	{
		QString clipperName = clipperNames.at(i);
		if(clippers->getClipper(clipperName)->exists(activeData))
			mDataCheckBoxes[clipperName]->setChecked(true);
		else
			mDataCheckBoxes[clipperName]->setChecked(false);
	}
}

void SelectClippersForDataWidget::clipDataClicked(bool checked)
{
	this->addDataToClippers();
}

void SelectClippersForDataWidget::addDataToClippers()
{
	ClippersPtr clippers = mServices->view()->getClippers();
	DataPtr activeData = mActiveData->getActive<Data>();

	QMap<QString, QCheckBox*>::const_iterator iter = mDataCheckBoxes.constBegin();
	 while (iter != mDataCheckBoxes.constEnd())
	 {
		 cx::InteractiveClipperPtr clipper = clippers->getClipper(iter.key());
//		 CX_LOG_DEBUG() << "clipper: " << iter.key() << " " << iter.value()->isChecked();
		 if(iter.value()->isChecked())
			 clipper->addData(activeData);
		 else
			 clipper->removeData(activeData);
		 ++iter;
	 }
}
}//cx

