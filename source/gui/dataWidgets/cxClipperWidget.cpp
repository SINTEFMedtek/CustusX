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

#include <QListWidget>
#include <QTableWidget>
#include <QGroupBox>
#include "cxClipperWidget.h"
#include "cxClippingWidget.h" //Using StringPropertyClipPlane. Fix
#include "cxLabeledComboBoxWidget.h"
#include "cxInteractiveClipper.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxLogger.h"
#include "cxStringPropertySelectTool.h"
#include "cxSelectDataStringPropertyBase.h"

namespace cx
{

ClipperWidget::ClipperWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ClipperWidget", "Clipper"),
	mServices(services),
	mInitializedWithClipper(false),
	planeSelector(NULL)
{
	this->setEnabled(false);
	this->setupDataStructures();

	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &OptimizedUpdateWidget::setModified);
}

void ClipperWidget::setupDataStructures()
{
	mLayout = new QVBoxLayout(this);
	mUseClipperCheckBox = new QCheckBox("Enable");
	mUseClipperCheckBox->setToolTip("Enable/disable interactive clipper.");
	mUseClipperCheckBox->setChecked(true);

	mAttachedToTool = new QCheckBox("Attach to tool");
	mAttachedToTool->setEnabled(true);
	mSelectAllData = new QCheckBox("Select all");
	mInvertPlane = new QCheckBox("Invert plane");

	mShowImages = new QCheckBox("Images");
	mShowMeshes = new QCheckBox("Meshes");
	mShowMetrics = new QCheckBox("Metrics");
	mShowTrackedStreams = new QCheckBox("TrackedStreams");
	mShowImages->setChecked(true);
	mShowMeshes->setChecked(true);

	connect(mSelectAllData, &QCheckBox::clicked, this, &ClipperWidget::selectAllTableData);

	connect(mShowImages, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mShowMeshes, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mShowMetrics, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mShowTrackedStreams, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);

	//TODO: Need active tool in addition to tool list. Also need to connect this to mClipper
	mToolSelector = StringPropertySelectTool::New(mServices->tracking());
	mToolSelector->setValueName("Tool");
	mDataTableWidget = new QTableWidget(this);
}

void ClipperWidget::setupUI()
{
	if(!mClipper || mInitializedWithClipper)
		return;
	mInitializedWithClipper = true;

	mLayout->addLayout(this->planeLayout());
	mLayout->addLayout(this->toolLayout());
	mLayout->addWidget(this->dataTableWidget());
	mLayout->addWidget(mUseClipperCheckBox);

	mLayout->addStretch();

	if(mClipper)
		connect(mUseClipperCheckBox, &QCheckBox::toggled, this, &ClipperWidget::enable);

}

void ClipperWidget::enable(bool checked)
{
	mClipper->useClipper(checked);
}

QLayout *ClipperWidget::planeLayout()
{
	mPlaneAdapter = StringPropertyClipPlane::New(mClipper);
	planeSelector = new LabeledComboBoxWidget(this, mPlaneAdapter);

	QHBoxLayout *layout = new QHBoxLayout();

	layout->addWidget(planeSelector);
	layout->addWidget(mInvertPlane);
	return layout;
}


QLayout *ClipperWidget::toolLayout()
{
	LabeledComboBoxWidget* toolSelectorWidget = new LabeledComboBoxWidget(this, mToolSelector);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(toolSelectorWidget);
	layout->addWidget(mAttachedToTool);
	return layout;
}


QGroupBox *ClipperWidget::dataTableWidget()
{
	QGroupBox *groupBox = new QGroupBox("Structures to clip");
	QVBoxLayout *layout = new QVBoxLayout();
	QHBoxLayout *selectCheckBoxes = new QHBoxLayout();

	selectCheckBoxes->addWidget(mShowImages);
	selectCheckBoxes->addWidget(mShowMeshes);
	selectCheckBoxes->addWidget(mShowTrackedStreams);
	selectCheckBoxes->addWidget(mShowMetrics);

	layout->addLayout(selectCheckBoxes);

	layout->addWidget(mDataTableWidget);
	layout->addWidget(mSelectAllData);

	groupBox->setLayout(layout);

	return groupBox;
}

void ClipperWidget::connectToNewClipper()
{
	if(mClipper)
	{
		mUseClipperCheckBox->setChecked(mClipper->getUseClipper());
		mInvertPlane->setChecked(mClipper->getInvertPlane());
		connect(mUseClipperCheckBox, &QCheckBox::toggled, this, &ClipperWidget::enable);
		connect(mInvertPlane, &QCheckBox::toggled, mClipper.get(), &InteractiveClipper::invertPlane);
		if(planeSelector)
		{
			mPlaneAdapter->setClipper(mClipper);
			planeSelector->setModified();
		}
		this->setEnabled(true);
		this->setupUI();
		this->setupDataSelectorUI();
	}
	else
		this->setEnabled(false);

}

void ClipperWidget::setClipper(InteractiveClipperPtr clipper)
{
	if(mClipper)
	{
		disconnect(mUseClipperCheckBox, &QCheckBox::toggled, this, &ClipperWidget::enable);
		disconnect(mInvertPlane, &QCheckBox::toggled, mClipper.get(), &InteractiveClipper::invertPlane);
	}

	mClipper = clipper;

	this->connectToNewClipper();
}

void ClipperWidget::initCheckboxesBasedOnClipper()
{
	std::map<QString, DataPtr> datas = mClipper->getDatas();
	std::map<QString, DataPtr>::iterator iter = datas.begin();
	for(; iter != datas.end(); ++iter)
	{
		if(mCheckBoxes.contains(iter->first))
			mCheckBoxes[iter->first]->setChecked(true);
	}
}

void ClipperWidget::createNewCheckboxesBasedOnData()
{
	mCheckBoxes.clear();

	std::map<QString, DataPtr> datas = this->getDatas();
	std::map<QString, DataPtr>::iterator iter = datas.begin();

	int row = 0;

	for(; iter != datas.end(); ++iter)
	{
		DataPtr data = iter->second;
		QCheckBox *checkbox = new QCheckBox();
		checkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		mDataTableWidget->setCellWidget(row, 0, checkbox);

		QTableWidgetItem *descriptionItem = new QTableWidgetItem(data->getName());
		mDataTableWidget->setItem(row++, 1, descriptionItem);

		connect(checkbox, &QCheckBox::clicked, this, &ClipperWidget::dataSelectorClicked);
		mCheckBoxes[data->getUid()] = checkbox;
	}
}

void ClipperWidget::setupDataSelectorUI()
{
	if(!mClipper)
	{
		this->setEnabled(false);
		return;
	}

	std::map<QString, DataPtr> datas = this->getDatas();

	mDataTableWidget->setColumnCount(2);;
	mDataTableWidget->setRowCount(datas.size());

	QStringList horizontalHeaders;
	horizontalHeaders << "Apply clipper" << "Object to be clipped";
	mDataTableWidget->setHorizontalHeaderLabels(horizontalHeaders);
	mDataTableWidget->setColumnWidth(1, 300);
//	mDataTableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);//test

	createNewCheckboxesBasedOnData();
	initCheckboxesBasedOnClipper();
}

QString ClipperWidget::getDataTypeRegExp()
{
	QStringList dataTypes;
	if(mShowImages->isChecked())
		dataTypes << "image";
	if(mShowMeshes->isChecked())
		dataTypes << "mesh";
	if(mShowTrackedStreams->isChecked())
		dataTypes << "trackedStream";
	if(mShowMetrics->isChecked())
		dataTypes << ".*Metric$";
	QString typeRegExp = dataTypes.join('|');

	return typeRegExp;
}

std::map<QString, DataPtr> ClipperWidget::getDatas()
{
	//TODO: Move SelectDataStringPropertyBase::filterOnType() to a utility file?
	std::map<QString, DataPtr> datas = mServices->patient()->getData();
	datas = SelectDataStringPropertyBase::filterOnType(datas, this->getDataTypeRegExp());
	return datas;
}

void ClipperWidget::dataSelectorClicked(bool checked)
{
	if(!checked)
		mSelectAllData->setChecked(false);
	this->setClipPlaneInDatas();
}

void ClipperWidget::setClipPlaneInDatas()
{
	if(!mClipper)
		return;
	QMap<QString, QCheckBox*>::const_iterator iter = mCheckBoxes.constBegin();
	 while (iter != mCheckBoxes.constEnd())
	 {
		 DataPtr data = mServices->patient()->getData(iter.key());
		 if(iter.value()->isChecked())
			 mClipper->addData(data);
		 else
			 mClipper->removeData(data);
		 ++iter;
	 }

}

void ClipperWidget::removeAllClipPlanes()
{
	QMap<QString, QCheckBox*>::const_iterator iter = mCheckBoxes.constBegin();
	 while (iter != mCheckBoxes.constEnd())
	 {
		 DataPtr data = mServices->patient()->getData(iter.key());
		 mClipper->removeData(data);
		 ++iter;
	 }
}

void ClipperWidget::selectAllTableData(bool checked)
{
	this->removeAllClipPlanes();

	QMap<QString, QCheckBox*>::const_iterator iter = mCheckBoxes.constBegin();
	 while (iter != mCheckBoxes.constEnd())
	 {
		 iter.value()->setChecked(checked);
		 ++iter;
	 }

	 this->setClipPlaneInDatas();
}

void ClipperWidget::dataTypeSelectorClicked(bool checked)
{
	if(checked)
		mSelectAllData->setChecked(false);

	this->removeAllClipPlanes();
	this->setupDataSelectorUI();
	this->setClipPlaneInDatas();
}

void ClipperWidget::prePaintEvent()
{
	this->setupDataSelectorUI();
}

}//cx
