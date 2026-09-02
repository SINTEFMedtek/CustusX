/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLiverVisibilityWidget.h"

#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>

#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxViewGroupData.h"
#include "cxMesh.h"
#include "cxStyles.h"

namespace cx
{

SelectableLiverStructure::SelectableLiverStructure() :
	mButton(nullptr),
	mViewEnabled(false)
{
}

LiverVisibilityWidget::LiverVisibilityWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, this->getWidgetName(), "Liver Visibility"),
	mServices(services)
{
	QGridLayout* structuresLayout = new QGridLayout();
	int row = 0;
	this->addStructureButton(otLIVER, "Liver", structuresLayout, row++);
	this->addStructureButton(otPANCREAS, "Pancreas", structuresLayout, row++);
	this->addStructureButton(otLIVER_VESSELS, "Liver Vessels", structuresLayout, row++);
	this->addStructureButton(otLIVER_LESIONS, "Liver Lesions", structuresLayout, row++);
	QGroupBox* structuresGroup = new QGroupBox("Liver Structures");
	structuresGroup->setLayout(structuresLayout);

	QGridLayout* segmentsLayout = new QGridLayout();
	int segmentRow = 0;
	ORGAN_TYPE segments[8] = {otLIVER_SEGMENT_1, otLIVER_SEGMENT_2, otLIVER_SEGMENT_3, otLIVER_SEGMENT_4,
	                          otLIVER_SEGMENT_5, otLIVER_SEGMENT_6, otLIVER_SEGMENT_7, otLIVER_SEGMENT_8};
	for (int i = 0; i < 8; ++i)
		this->addStructureButton(segments[i], QString("Segment %1").arg(i + 1), segmentsLayout, segmentRow++);
	QGroupBox* segmentsGroup = new QGroupBox("Liver Segments (Couinaud)");
	segmentsGroup->setLayout(segmentsLayout);

	QPushButton* refreshButton = new QPushButton("Refresh");
	connect(refreshButton, &QPushButton::clicked, this, &LiverVisibilityWidget::refreshStructures);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(refreshButton);
	layout->addWidget(structuresGroup);
	layout->addWidget(segmentsGroup);
	layout->addStretch();
	this->setLayout(layout);

	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &LiverVisibilityWidget::refreshStructures);
	this->refreshStructures();
}

LiverVisibilityWidget::~LiverVisibilityWidget()
{
}

QString LiverVisibilityWidget::getWidgetName()
{
	return "liver_visibility_widget";
}

void LiverVisibilityWidget::addStructureButton(ORGAN_TYPE organType, QString label, QGridLayout* layout, int row)
{
	SelectableLiverStructure structure;
	structure.mName = label;
	structure.mButton = new QPushButton(label);
	structure.mButton->setEnabled(false);
	layout->addWidget(structure.mButton, row, 0);
	mStructures.insert(organType, structure);

	connect(structure.mButton, &QPushButton::clicked, this, [=]() { this->toggleStructure(organType); });
}

void LiverVisibilityWidget::refreshStructures()
{
	QMapIterator<ORGAN_TYPE, SelectableLiverStructure> i(mStructures);
	while (i.hasNext())
	{
		i.next();
		ORGAN_TYPE organType = i.key();
		MeshPtr mesh = mServices->patient()->getData<Mesh>(organType);
		mStructures[organType].mMesh = mesh;
		mStructures[organType].mButton->setEnabled(mesh != nullptr);
		this->updateButtonColor(organType);
	}
}

void LiverVisibilityWidget::toggleStructure(ORGAN_TYPE organType)
{
	SelectableLiverStructure& structure = mStructures[organType];
	if (!structure.mMesh)
		return;

	structure.mViewEnabled = !structure.mViewEnabled;
	if (structure.mViewEnabled)
		this->showMesh(structure.mMesh);
	else
		this->hideMesh(structure.mMesh);

	this->updateButtonColor(organType);
}

void LiverVisibilityWidget::updateButtonColor(ORGAN_TYPE organType)
{
	SelectableLiverStructure& structure = mStructures[organType];
	QPalette palette = structure.mButton->palette();
	palette.setColor(QPalette::Button, structure.mViewEnabled ? Styles::getGreen() : Styles::getRed());
	structure.mButton->setPalette(palette);
}

void LiverVisibilityWidget::showMesh(MeshPtr mesh)
{
	for (unsigned i = 0; i < mServices->view()->groupCount(); ++i)
	{
		ViewGroupDataPtr viewGroup = mServices->view()->getGroup(i);
		if (viewGroup)
			viewGroup->addData(mesh->getUid());
	}
}

void LiverVisibilityWidget::hideMesh(MeshPtr mesh)
{
	for (unsigned i = 0; i < mServices->view()->groupCount(); ++i)
	{
		ViewGroupDataPtr viewGroup = mServices->view()->getGroup(i);
		if (viewGroup)
			viewGroup->removeData(mesh->getUid());
	}
}

} /* namespace cx */
