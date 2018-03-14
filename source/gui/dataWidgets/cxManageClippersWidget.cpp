/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxManageClippersWidget.h"
#include <QPushButton>
#include "cxLabeledComboBoxWidget.h"
#include "cxInteractiveClipper.h"
#include "cxVisServices.h"
#include "cxLogger.h"
#include "cxProfile.h"
#include "cxClippers.h"
#include "cxClipperWidget.h"
#include "cxViewService.h"
#include "cxSelectClippersForDataWidget.h"

namespace cx
{

ClippingPropertiesWidget::ClippingPropertiesWidget(VisServicesPtr services, QWidget *parent) :
		TabbedWidget(parent, "ClippingPropertiesWidget", "Clipping Properties")
{
	this->setToolTip("Clipping properties");

	this->addTab(new ManageClippersWidget(services, this), "Edit clippers");
}

ManageClippersWidget::ManageClippersWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ManageClippersWidget", "Manage Clippers"),
	mServices(services),
	mClipperWidget(new ClipperWidget(services, this))
{
	this->setupUI();
	connect(getClippers().get(), &Clippers::changed, this, &ManageClippersWidget::clippersChanged);

}

ClippersPtr ManageClippersWidget::getClippers()
{
	return mServices->view()->getClippers();
}

void ManageClippersWidget::initClipperSelector()
{
	XmlOptionFile mOptions = profile()->getXmlSettings().descend("clippers");

	QStringList range = getClippers()->getClipperNames();
	mClipperSelector = StringProperty::initialize("clipperSelector", "Clipper", "Select clipper", "", range, mOptions.getElement());
	connect(mClipperSelector.get(), &Property::changed, this, &ManageClippersWidget::clipperChanged);
	this->clippersChanged();
}

void ManageClippersWidget::setupUI()
{
	initClipperSelector();
	LabeledComboBoxWidget* clipperSelectorBox = new LabeledComboBoxWidget(this, mClipperSelector);

	QPushButton* newClipperButton = new QPushButton("Create clipper", this);
	newClipperButton->setToolTip("Create new clipper based on currently selected");
	newClipperButton->setEnabled(false);//TODO: Disabled for now

	mLayout = new QVBoxLayout(this);
	mLayout->addWidget(newClipperButton);
	mLayout->addWidget(clipperSelectorBox);

	connect(newClipperButton, &QPushButton::clicked, this, &ManageClippersWidget::newClipperButtonClicked);


	mLayout->addWidget(mClipperWidget);

	mLayout->addStretch();
}

void ManageClippersWidget::clippersChanged()
{
	mClipperSelector->setValueRange(getClippers()->getClipperNames());

	QStringList range = mClipperSelector->getValueRange();
	if(!range.contains(mClipperSelector->getValue()))
	{
		if(range.isEmpty())
			mClipperSelector->setValue("");
		else
			mClipperSelector->setValue(range.first());
	}
}

void ManageClippersWidget::clipperChanged()
{
	QString clipperName = mClipperSelector->getValue();
	if(clipperName.isEmpty())
		return;

	mCurrentClipper = getClippers()->getClipper(clipperName);
	mClipperWidget->setClipper(mCurrentClipper);
}

void ManageClippersWidget::newClipperButtonClicked()
{
	InteractiveClipperPtr interactiveClipper = InteractiveClipperPtr(new InteractiveClipper(mServices));
	QString nameBase = getNameBaseOfCurrentClipper();
	QString clipperName = nameBase;

	int i = 2;
	while(getClippers()->exists(clipperName))
		clipperName = nameBase + QString(" %1").arg(i++);

	getClippers()->add(clipperName, interactiveClipper);

	QStringList range = mClipperSelector->getValueRange();
	range << clipperName;
	mClipperSelector->setValueRange(range);
	mClipperSelector->setValue(clipperName);
}

QString ManageClippersWidget::getNameBaseOfCurrentClipper()
{
	QString nameBase = mClipperSelector->getValue();
	nameBase.remove(QRegExp(" [0-9]{1,2}$"));

	return nameBase;
}

} //cx