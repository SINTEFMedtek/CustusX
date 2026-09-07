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
#include "cxLiverSegmentationWidget.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxViewGroupData.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxSelectDataStringProperty.h"
#include "cxDataSelectWidget.h"
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
	mServices(services),
	mSourceImageSelector(StringPropertyActiveImage::New(services->patient())),
	mAllSegmentsButton(nullptr),
	mAllSegmentsViewEnabled(false),
	mSourceVolumeButton(nullptr),
	mSourceVolumeViewEnabled(false)
{
	mSourceImageSelector->setValueName("Source Image (Active)");
	mSourceImageSelector->setHelp("The active volume - shows/hides its segmented structures");
	connect(mSourceImageSelector.get(), &SelectDataStringPropertyBase::dataChanged, this, &LiverVisibilityWidget::refreshStructures);

	QGridLayout* imageSelectorLayout = new QGridLayout();
	new DataSelectWidget(mServices->view(), mServices->patient(), this, mSourceImageSelector, imageSelectorLayout, 0);
	// Column 1 is the combobox itself (0=label, 2=show/remove buttons) -
	// stretch it to show more of the volume's name, matching how
	// ActiveVolumeWidget's combobox (used in "Volume Properties") expands
	// to fill its own layout.
	imageSelectorLayout->setColumnStretch(1, 1);

	mSourceVolumeButton = new QPushButton("Source Volume");
	connect(mSourceVolumeButton, &QPushButton::clicked, this, &LiverVisibilityWidget::toggleSourceVolume);

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
	mSegmentOrganTypes = LiverSegmentationWidget::organTypesFor(LiverSegmentationWidget::fkLiverSegments);
	for (int i = 0; i < mSegmentOrganTypes.size(); ++i)
		this->addStructureButton(mSegmentOrganTypes[i], QString("Segment %1").arg(i + 1), segmentsLayout, segmentRow++);

	mAllSegmentsButton = new QPushButton("Show/Hide All Segments");
	connect(mAllSegmentsButton, &QPushButton::clicked, this, &LiverVisibilityWidget::toggleAllSegments);
	segmentsLayout->addWidget(mAllSegmentsButton, segmentRow++, 0);

	QGroupBox* segmentsGroup = new QGroupBox("Liver Segments (Couinaud)");
	segmentsGroup->setLayout(segmentsLayout);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(imageSelectorLayout);
	layout->addWidget(mSourceVolumeButton);
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

ImagePtr LiverVisibilityWidget::sourceImage() const
{
	return boost::dynamic_pointer_cast<Image>(mSourceImageSelector->getData());
}

bool LiverVisibilityWidget::descendsFrom(PatientModelServicePtr patient, QString uid, QString ancestorUid)
{
	// Walk the parent-frame chain: a mesh produced from a cropped/resampled
	// copy of the source image (see LiverSegmentationWidget::
	// prepareImageForHeavyFilter()) is parented to that copy, not directly
	// to the source image, so an exact-match comparison would miss it.
	for (int steps = 0; !uid.isEmpty() && steps < 10; ++steps)
	{
		if (uid == ancestorUid)
			return true;
		DataPtr data = patient->getData(uid);
		uid = data ? data->getParentSpace() : QString();
	}
	return false;
}

MeshPtr LiverVisibilityWidget::findMeshForSourceImage(ORGAN_TYPE organType, ImagePtr sourceImage) const
{
	if (!sourceImage)
		return MeshPtr();

	std::map<QString, MeshPtr> candidates = mServices->patient()->getDataOfType<Mesh>(organType);
	std::map<QString, MeshPtr>::iterator it;
	for (it = candidates.begin(); it != candidates.end(); ++it)
		if (it->second && this->descendsFrom(mServices->patient(), it->second->getParentSpace(), sourceImage->getUid()))
			return it->second;
	return MeshPtr();
}

bool LiverVisibilityWidget::isShown(QString uid) const
{
	if (mServices->view()->groupCount() == 0)
		return false;

	ViewGroupDataPtr viewGroup = mServices->view()->getGroup(0);
	if (!viewGroup)
		return false;

	std::vector<DataPtr> visibleData = viewGroup->getData();
	for (size_t i = 0; i < visibleData.size(); ++i)
		if (visibleData[i] && visibleData[i]->getUid() == uid)
			return true;
	return false;
}

void LiverVisibilityWidget::refreshStructures()
{
	ImagePtr sourceImage = this->sourceImage();

	QMapIterator<ORGAN_TYPE, SelectableLiverStructure> i(mStructures);
	while (i.hasNext())
	{
		i.next();
		ORGAN_TYPE organType = i.key();
		MeshPtr mesh = this->findMeshForSourceImage(organType, sourceImage);
		mStructures[organType].mMesh = mesh;
		mStructures[organType].mViewEnabled = mesh && this->isShown(mesh->getUid());
		mStructures[organType].mButton->setEnabled(mesh != nullptr);
		this->updateButtonColor(organType);
	}

	bool anySegmentPresent = false;
	for (int i = 0; i < mSegmentOrganTypes.size(); ++i)
		anySegmentPresent |= (mStructures[mSegmentOrganTypes[i]].mMesh != nullptr);
	if (mAllSegmentsButton)
		mAllSegmentsButton->setEnabled(anySegmentPresent);

	mSourceVolumeViewEnabled = sourceImage && this->isShown(sourceImage->getUid());
	mSourceVolumeButton->setEnabled(sourceImage != nullptr);
	this->updateSourceVolumeButtonColor();
}

void LiverVisibilityWidget::toggleStructure(ORGAN_TYPE organType)
{
	SelectableLiverStructure& structure = mStructures[organType];
	if (!structure.mMesh)
		return;

	structure.mViewEnabled = !structure.mViewEnabled;
	if (structure.mViewEnabled)
		this->showData(structure.mMesh->getUid());
	else
		this->hideData(structure.mMesh->getUid());

	this->updateButtonColor(organType);
}

void LiverVisibilityWidget::updateButtonColor(ORGAN_TYPE organType)
{
	SelectableLiverStructure& structure = mStructures[organType];
	QPalette palette = structure.mButton->palette();
	palette.setColor(QPalette::Button, structure.mViewEnabled ? Styles::getGreen() : Styles::getRed());
	structure.mButton->setPalette(palette);
}

void LiverVisibilityWidget::toggleAllSegments()
{
	mAllSegmentsViewEnabled = !mAllSegmentsViewEnabled;
	for (int i = 0; i < mSegmentOrganTypes.size(); ++i)
	{
		ORGAN_TYPE organType = mSegmentOrganTypes[i];
		SelectableLiverStructure& structure = mStructures[organType];
		if (!structure.mMesh)
			continue;

		structure.mViewEnabled = mAllSegmentsViewEnabled;
		if (structure.mViewEnabled)
			this->showData(structure.mMesh->getUid());
		else
			this->hideData(structure.mMesh->getUid());
		this->updateButtonColor(organType);
	}
	this->updateAllSegmentsButtonColor();
}

void LiverVisibilityWidget::updateAllSegmentsButtonColor()
{
	if (!mAllSegmentsButton)
		return;

	QPalette palette = mAllSegmentsButton->palette();
	palette.setColor(QPalette::Button, mAllSegmentsViewEnabled ? Styles::getGreen() : Styles::getRed());
	mAllSegmentsButton->setPalette(palette);
}

void LiverVisibilityWidget::toggleSourceVolume()
{
	ImagePtr sourceImage = this->sourceImage();
	if (!sourceImage)
		return;

	mSourceVolumeViewEnabled = !mSourceVolumeViewEnabled;
	if (mSourceVolumeViewEnabled)
		this->showData(sourceImage->getUid());
	else
		this->hideData(sourceImage->getUid());

	this->updateSourceVolumeButtonColor();
}

void LiverVisibilityWidget::updateSourceVolumeButtonColor()
{
	QPalette palette = mSourceVolumeButton->palette();
	palette.setColor(QPalette::Button, mSourceVolumeViewEnabled ? Styles::getGreen() : Styles::getRed());
	mSourceVolumeButton->setPalette(palette);
}

void LiverVisibilityWidget::showData(QString uid)
{
	for (unsigned i = 0; i < mServices->view()->groupCount(); ++i)
	{
		ViewGroupDataPtr viewGroup = mServices->view()->getGroup(i);
		if (viewGroup)
			viewGroup->addData(uid);
	}
}

void LiverVisibilityWidget::hideData(QString uid)
{
	for (unsigned i = 0; i < mServices->view()->groupCount(); ++i)
	{
		ViewGroupDataPtr viewGroup = mServices->view()->getGroup(i);
		if (viewGroup)
			viewGroup->removeData(uid);
	}
}

} /* namespace cx */
