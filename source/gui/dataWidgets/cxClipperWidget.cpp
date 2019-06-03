/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "boost/bind.hpp"
#include <QListWidget>
#include <QTableWidget>
#include <QGroupBox>
#include "cxClipperWidget.h"
#include "cxStringPropertyClipPlane.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxInteractiveClipper.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxTrackedStream.h"
#include "cxLogger.h"
#include "cxStringPropertySelectTool.h"
#include "cxSelectDataStringPropertyBase.h"
#include "cxTrackingService.h"

namespace cx
{

ClipperWidget::ClipperWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ClipperWidget", "Clipper"),
	mServices(services),
	mInitializedWithClipper(false)
//	planeSelector(NULL)
{
	this->setEnabled(false);
	this->setupDataStructures();
	createNewCheckboxesBasedOnData();

	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &OptimizedUpdateWidget::setModified);
}

ClipperWidget::~ClipperWidget()
{
	disconnect(mSelectAllData, &QCheckBox::clicked, this, &ClipperWidget::selectAllTableData);
	disconnect(mShowImages, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mShowMeshes, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mShowMetrics, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mShowTrackedStreams, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mUseClipperCheckBox, &QCheckBox::toggled, this, &ClipperWidget::enable);
	connect(mToolSelector.get(), &StringPropertySelectTool::changed, this, &ClipperWidget::onToolChanged);

	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &OptimizedUpdateWidget::setModified);
}

void ClipperWidget::setupDataStructures()
{
	mLayout = new QVBoxLayout(this);
	mUseClipperCheckBox = new QCheckBox("Enable");
	mUseClipperCheckBox->setToolTip("Enable/disable interactive clipper.");
	mUseClipperCheckBox->setChecked(true);

	mAttachedToTool = new QCheckBox("Attach to tool");
	mAttachedToTool->setChecked(true);
	mSelectAllData = new QCheckBox("Select all");
	mInvertPlane = new QCheckBox("Invert plane");

	mShowImages = new QCheckBox("Images");
	mShowMeshes = new QCheckBox("Meshes");
	mShowMetrics = new QCheckBox("Metrics");
	mShowTrackedStreams = new QCheckBox("TrackedStreams");
	mShowImages->setChecked(true);
	mShowMeshes->setChecked(true);

	mToolSelector = StringPropertySelectTool::New(mServices->tracking());
	mToolSelector->setValueName("Tool");
	mToolSelector->provideActiveTool(true);
	mToolSelector->setActiveTool();

	connect(mSelectAllData, &QCheckBox::clicked, this, &ClipperWidget::selectAllTableData);

	connect(mShowImages, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mShowMeshes, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mShowMetrics, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);
	connect(mShowTrackedStreams, &QCheckBox::clicked, this, &ClipperWidget::dataTypeSelectorClicked);

	connect(mUseClipperCheckBox, &QCheckBox::toggled, this, &ClipperWidget::enable);
	connect(mToolSelector.get(), &StringPropertySelectTool::changed, this, &ClipperWidget::onToolChanged);
	connect(mAttachedToTool, &QCheckBox::toggled, this, &ClipperWidget::onToolChanged);
	connect(mServices->tracking().get(), &TrackingService::activeToolChanged, this, &ClipperWidget::onToolChanged);

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
	if(!mClipper)
		return;
	mClipper->useClipper(checked);
}

QLayout *ClipperWidget::planeLayout()
{
//	mPlaneAdapter = StringPropertyClipPlane::New(mClipper);
//	planeSelector = new LabeledComboBoxWidget(this, mPlaneAdapter);

	QHBoxLayout *layout = new QHBoxLayout();

//	layout->addWidget(planeSelector);
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
		connect(mInvertPlane, &QCheckBox::toggled, mClipper.get(), &InteractiveClipper::invertPlane);
//		if(planeSelector)
//		{
//			mPlaneAdapter->setClipper(mClipper);
//			planeSelector->setModified();
//		}
		this->setEnabled(true);
		this->setupUI();
		this->setupDataSelectorUI();
	}
	else
		this->setEnabled(false);

}

void ClipperWidget::onToolChanged()
{
	if(!mClipper)
		return;
	ToolPtr tool = mToolSelector->getTool();
	if(!tool)
	{
		mClipper->useActiveTool(true);
		tool = mServices->tracking()->getActiveTool();
	}
	else
		mClipper->useActiveTool(false);
	if(mAttachedToTool->isChecked())
		mClipper->setTool(tool);
	else
		mClipper->setTool(ToolPtr());
}

void ClipperWidget::setClipper(InteractiveClipperPtr clipper)
{
	if(mClipper)
	{
		disconnect(mInvertPlane, &QCheckBox::toggled, mClipper.get(), &InteractiveClipper::invertPlane);
	}

	mClipper = clipper;

	this->connectToNewClipper();
}

void ClipperWidget::updateSelectAllCheckbox()
{
	if(this->getDatas().size() == mClipper->getDatas().size())
		mSelectAllData->setChecked(true);
	else
		mSelectAllData->setChecked(false);
}

void ClipperWidget::updateCheckBoxFromClipper(QCheckBox *checkbox, DataPtr data)
{
	if(!mClipper)
		return;
	std::map<QString, DataPtr> datas = mClipper->getDatas();
	bool checked = datas.count(data->getUid());
	checkbox->setChecked(checked);
}

void ClipperWidget::createNewCheckboxesBasedOnData()
{
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

		boost::function<void()> func = boost::bind(&ClipperWidget::dataSelectorClicked, this, checkbox, data);
		connect(checkbox, &QCheckBox::clicked, this, func);
		this->updateCheckBoxFromClipper(checkbox, data);
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

	mDataTableWidget->setColumnCount(2);
	mDataTableWidget->setRowCount(datas.size());

	QStringList horizontalHeaders;
	horizontalHeaders << "Apply clipper" << "Object to be clipped";
	mDataTableWidget->setHorizontalHeaderLabels(horizontalHeaders);
	mDataTableWidget->setColumnWidth(1, 300);
//	mDataTableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);//test

	createNewCheckboxesBasedOnData();
	updateSelectAllCheckbox();
}

QString ClipperWidget::getDataTypeRegExp()
{
	QStringList dataTypes;
	if(mShowImages->isChecked())
		dataTypes << Image::getTypeName();
	if(mShowMeshes->isChecked())
		dataTypes << Mesh::getTypeName();
	if(mShowTrackedStreams->isChecked())
		dataTypes << TrackedStream::getTypeName();
	if(mShowMetrics->isChecked())
		dataTypes << ".*Metric$";
	QString typeRegExp = dataTypes.join('|');

	return typeRegExp;
}

std::map<QString, DataPtr> ClipperWidget::getDatas()
{
	//TODO: Move SelectDataStringPropertyBase::filterOnType() to a utility file?
	std::map<QString, DataPtr> datas = mServices->patient()->getDatas();
	datas = SelectDataStringPropertyBase::filterOnType(datas, this->getDataTypeRegExp());
	return datas;
}

void ClipperWidget::dataSelectorClicked(QCheckBox *checkBox, DataPtr data)
{
	bool checked = checkBox->isChecked();

	if(checked)
		mClipper->addData(data);
	else
		mClipper->removeData(data);
}

void ClipperWidget::selectAllTableData(bool checked)
{
	std::map<QString, DataPtr> datas = this->getDatas();
	std::map<QString, DataPtr>::iterator iter = datas.begin();

	for(; iter != datas.end(); ++iter)
	{
		DataPtr data = iter->second;
		if(checked)
			mClipper->addData(data);
		else
			mClipper->removeData(data);
	}
	this->setModified();
}

void ClipperWidget::dataTypeSelectorClicked(bool checked)
{
	this->setModified();
}

void ClipperWidget::prePaintEvent()
{
	this->setupDataSelectorUI();
}

}//cx
