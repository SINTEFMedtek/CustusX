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
//#include <QMap>
#include "cxClipperWidget.h"
#include "cxClippingWidget.h" //Using StringPropertyClipPlane. Fix
#include "cxLabeledComboBoxWidget.h"
#include "cxInteractiveClipper.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
//#include "cxData.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxLogger.h"

namespace cx
{

ClipperWidget::ClipperWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ClipperWidget", "Clipper"),
	mServices(services)
{
	this->setupUI();

	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &OptimizedUpdateWidget::setModified);
}

void ClipperWidget::setupUI()
{
//	QGroupBox* activeClipGroupBox = new QGroupBox("Interactive clipper");
//	activeClipGroupBox->setToolTip(this->toolTip());
//	layout->addWidget(activeClipGroupBox);
//	QVBoxLayout* activeClipLayout = new QVBoxLayout(activeClipGroupBox);


	mLayout = new QVBoxLayout(this);

	//Skip plane selector for now
//	mPlaneAdapter = StringPropertyClipPlane::New(mClipper);
//	LabeledComboBoxWidget* planeSelector = new LabeledComboBoxWidget(this, mPlaneAdapter);
//	mLayout->addWidget(planeSelector);


	mUseClipperCheckBox = new QCheckBox("Enable");
	mUseClipperCheckBox->setToolTip("Enable/disable interactive clipper.");

	mLayout->addWidget(mUseClipperCheckBox);


	mDataTableWidget = new QTableWidget(this);
	mLayout->addWidget(mDataTableWidget);

	mLayout->addStretch();

	if(mClipper)
		connect(mUseClipperCheckBox, &QCheckBox::toggled, mClipper.get(), &InteractiveClipper::useClipper);

}

void ClipperWidget::setClipper(InteractiveClipperPtr clipper)
{
	if(mClipper)
		disconnect(mUseClipperCheckBox, &QCheckBox::toggled, mClipper.get(), &InteractiveClipper::useClipper);

	mClipper = clipper;

	connect(mUseClipperCheckBox, &QCheckBox::toggled, mClipper.get(), &InteractiveClipper::useClipper);

	this->setupDataSelectorUI();
}

void ClipperWidget::setupDataSelectorUI()
{
	if(!mClipper)
		return;

	std::map<QString, DataPtr> datas = this->getDatas();

	mDataTableWidget->setColumnCount(2);;
	mDataTableWidget->setRowCount(datas.size());

	QStringList horizontalHeaders;
	horizontalHeaders << "Apply clipper" << "Object to be clipped";
	mDataTableWidget->setHorizontalHeaderLabels(horizontalHeaders);
	mDataTableWidget->setColumnWidth(1, 300);
//	mDataTableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);//test

	int row = 0;
	std::map<QString, DataPtr>::iterator iter = datas.begin();

	mCheckBoxes.clear();

	for(; iter != datas.end(); ++iter)
	{
		DataPtr data = iter->second;
		QCheckBox *checkbox = new QCheckBox();
		checkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		mDataTableWidget->setCellWidget(row, 0, checkbox);

		QTableWidgetItem *descriptionItem = new QTableWidgetItem(data->getName());
		mDataTableWidget->setItem(row++, 1, descriptionItem);

		connect(checkbox, &QCheckBox::clicked, this, &ClipperWidget::setClipPlaneInDatas);
		mCheckBoxes[data->getUid()] = checkbox;
	}
}


std::map<QString, DataPtr> ClipperWidget::getDatas()
{
	std::map<QString, DataPtr> datas = mServices->patient()->getData();
//	std::map<QString, ImagePtr> images = mServices->patient()->getDataOfType<Image>();
//	std::map<QString, MeshPtr> meshes = mServices->patient()->getDataOfType<Mesh>();

//	std::map<QString, DataPtr> datas;

//	std::map<QString, ImagePtr>::iterator imageIter;
//	for(imageIter = images.begin(); imageIter != images.end(); ++imageIter)
//	{
//		datas[imageIter->first] = imageIter->second;
//	}

//	std::map<QString, MeshPtr>::iterator meshIter;
//	for(meshIter = meshes.begin(); meshIter != meshes.end(); ++meshIter)
//	{
//		datas[meshIter->first] = meshIter->second;
//	}


	//TODO: Use SelectDataStringPropertyBase::filterOnType() to filter data on type

	return datas;
}

void ClipperWidget::setClipPlaneInDatas()
{

	QMap<QString, QCheckBox*>::const_iterator iter = mCheckBoxes.constBegin();
	 while (iter != mCheckBoxes.constEnd())
	 {
		 DataPtr data = mServices->patient()->getData(iter.key());
		 if(iter.value()->isChecked())
			 mClipper->addClipPlaneToData(data);
		 else
			 mClipper->removeClipPlaneFromData(data);
		 ++iter;
	 }

}

void ClipperWidget::prePaintEvent()
{
	CX_LOG_DEBUG() << "ClipperWidget::prePaintEvent()";
	this->setupDataSelectorUI();
}

}//cx
