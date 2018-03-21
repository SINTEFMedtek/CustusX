/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSelectClippersForDataWidget.h"
#include "boost/bind.hpp"
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
SelectClippersForImageWidget::SelectClippersForImageWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "select_clippers_for_image_widget", "Select Clippers")
{
	StringPropertyActiveImagePtr activeImageProperty = StringPropertyActiveImage::New(services->patient());

	QVBoxLayout *mLayout = new QVBoxLayout(this);
	mLayout->setMargin(0);

	SelectClippersForDataWidget *selectClippersWidget = new SelectClippersForDataWidget(services, this);
	selectClippersWidget->setActiveDataProperty(activeImageProperty);

	mLayout->addWidget(selectClippersWidget);
}

/// -------------------------------------------------------

SelectClippersForMeshWidget::SelectClippersForMeshWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "select_clippers_for_mesh_widget", "Select Clippers")
{
	StringPropertyActiveDataPtr activeMeshProperty = StringPropertyActiveData::New(services->patient(), "mesh");

	QVBoxLayout *mLayout = new QVBoxLayout(this);
	mLayout->setMargin(0);

	SelectClippersForDataWidget *selectClippersWidget = new SelectClippersForDataWidget(services, this);
	selectClippersWidget->setActiveDataProperty(activeMeshProperty);

	mLayout->addWidget(selectClippersWidget);
}

/// -------------------------------------------------------

SelectClippersForDataWidget::SelectClippersForDataWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "select_clippers_for_data_widget", "Select Clippers"),
	mServices(services),
	mActiveDataProperty(StringPropertyActiveData::New(services->patient()))
{
	this->initUI();

	ClippersPtr clippers = mServices->view()->getClippers();
	connect(clippers.get(), &Clippers::changed, this, &SelectClippersForDataWidget::setModified);
	connect(mActiveDataProperty.get(), &Property::changed, this, &SelectClippersForDataWidget::setModified);
}

void SelectClippersForDataWidget::setActiveDataProperty(SelectDataStringPropertyBasePtr property)
{
	disconnect(mActiveDataProperty.get(), &Property::changed, this, &SelectClippersForDataWidget::setModified);
	mActiveDataProperty = property;
	connect(mActiveDataProperty.get(), &Property::changed, this, &SelectClippersForDataWidget::setModified);
}

void SelectClippersForDataWidget::initUI()
{
	mClipperTableWidget = new QTableWidget(this);

	mHeading = new QLabel("Active clippers");

	mLayout = new QVBoxLayout(this);
	mLayout->setMargin(0);
	mLayout->addWidget(mHeading);
	mLayout->addWidget(mClipperTableWidget);

	this->setupClipperSelectorUI();
	this->createNewCheckBoxesBasedOnClippers();
}

void SelectClippersForDataWidget::setupClipperSelectorUI()
{
	ClippersPtr clippers = mServices->view()->getClippers();
	mClipperTableWidget->setColumnCount(3);
	mClipperTableWidget->setRowCount(clippers->size());
	QStringList horizontalHeaders;
	horizontalHeaders << "Clip data" << "Clipper" << "Invert";
	mClipperTableWidget->setHorizontalHeaderLabels(horizontalHeaders);
}

void SelectClippersForDataWidget::createNewCheckBoxesBasedOnClippers()
{
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
	QCheckBox *dataCheckBox = this->createCheckBox(clipperName);
	QCheckBox *invertCheckbox = this->createCheckBox(clipperName);
	mClipperTableWidget->setCellWidget(row, 0, dataCheckBox);
	mClipperTableWidget->setCellWidget(row, 2, invertCheckbox);

	boost::function<void()> func = boost::bind(&SelectClippersForDataWidget::clipDataClicked, this, dataCheckBox, clipperName);
	connect(dataCheckBox, &QCheckBox::clicked, this, func);

	boost::function<void()> invertFunc = boost::bind(&SelectClippersForDataWidget::invertClicked, this, invertCheckbox, clipperName);
	connect(invertCheckbox, &QCheckBox::clicked, this, invertFunc);

	this->updateCheckBoxesFromClipper(dataCheckBox, invertCheckbox, clipperName);
}

void SelectClippersForDataWidget::updateCheckBoxesFromClipper(QCheckBox *dataCheckBox, QCheckBox *invertCheckBox, QString clipperName)
{
	cx::InteractiveClipperPtr clipper = this->getClipper(clipperName);
	DataPtr activeData = mActiveDataProperty->getData();

	bool checkData = clipper->exists(activeData);
	dataCheckBox->setChecked(checkData);

	bool checkInvert = clipper->getInvertPlane();
	invertCheckBox->setChecked(checkInvert);
}

QCheckBox *SelectClippersForDataWidget::createCheckBox(QString clipperName)
{
	QCheckBox *checkbox = new QCheckBox();
	checkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	return checkbox;
}

cx::InteractiveClipperPtr SelectClippersForDataWidget::getClipper(QString clipperName)
{
	ClippersPtr clippers = mServices->view()->getClippers();
	cx::InteractiveClipperPtr clipper = clippers->getClipper(clipperName);
	return clipper;
}

void SelectClippersForDataWidget::clipDataClicked(QCheckBox *checkBox, QString clipperName)
{
	DataPtr activeData = mActiveDataProperty->getData();
	cx::InteractiveClipperPtr clipper = this->getClipper(clipperName);
	bool checked = checkBox->isChecked();

	if(checked)
		clipper->addData(activeData);
	else
		clipper->removeData(activeData);
}

void SelectClippersForDataWidget::invertClicked(QCheckBox *checkBox, QString clipperName)
{
	bool checked = checkBox->isChecked();
	this->getClipper(clipperName)->invertPlane(checked);
}

void SelectClippersForDataWidget::prePaintEvent()
{
	this->createNewCheckBoxesBasedOnClippers();
}


}//cx

