/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSpaceEditWidget.h"
#include <iostream>
#include "cxHelperWidgets.h"
#include "cxLogger.h"
#include "cxEnumConversion.h"

namespace cx
{

SpaceEditWidget::SpaceEditWidget(QWidget* parent, SpacePropertyBasePtr dataInterface,
	QGridLayout* gridLayout, int row) :
	BaseWidget(parent, "SpaceEditWidget", "SpaceEditWidget")
{
	this->setToolTip("Edit a space (coordinate system)");
	CX_ASSERT(dataInterface->getAllowOnlyValuesInRange()==true);

	this->setEnabled(dataInterface->getEnabled());

	mData = dataInterface;
	connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

	mLabel = new QLabel(this);
	mLabel->setText(mData->getDisplayName());

	mIdCombo = new QComboBox(this);
	connect(mIdCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged()));

	mRefCombo = new QComboBox(this);
	connect(mRefCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged()));

	if (gridLayout) // add to input gridlayout
	{
		gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);
		gridLayout->addWidget(mIdCombo, row, 1);
		gridLayout->addWidget(mRefCombo, row, 2);
	}
	else // add directly to this
	{
		mTopLayout = new QHBoxLayout;
		mTopLayout->setMargin(0);
		this->setLayout(mTopLayout);

		mTopLayout->addWidget(mLabel);
		mTopLayout->addWidget(mIdCombo, 1);
		mTopLayout->addWidget(mRefCombo, 2);
	}

	this->setModified();
}

void SpaceEditWidget::attemptSetValue(COORDINATE_SYSTEM id, QString ref)
{
	Space space(id, ref);

	QStringList refs = this->getAvailableSpaceRefs(space.mId);
	if (refs.isEmpty())
		space.mRefObject = "";
	else if (!refs.contains(space.mRefObject))
		space.mRefObject = refs[0];


	std::vector<Space> range = mData->getValueRange();
	if (!count(range.begin(), range.end(), space))
	{
		this->setModified(); // repaint with old data
		return;
	}

	mData->setValue(space);
}

void SpaceEditWidget::comboIndexChanged()
{
	this->attemptSetValue(COORDINATE_SYSTEM(mIdCombo->currentData().toInt()), mRefCombo->currentData().toString());
}

void SpaceEditWidget::showLabel(bool on)
{
	mLabel->setVisible(on);
}


void SpaceEditWidget::rebuildIdCombobox()
{
	mIdCombo->clear();

	Space currentValue = mData->getValue();
	std::vector<COORDINATE_SYSTEM> ids = this->getAvailableSpaceIds();

	int currentIndex = -1;
	for (int i = 0; i < ids.size(); ++i)
	{
		COORDINATE_SYSTEM id = ids[i];
		QString name = enum2string<COORDINATE_SYSTEM>(id);

		mIdCombo->addItem(name);
		mIdCombo->setItemData(i, QVariant::fromValue<int>(id));
		if (id == currentValue.mId)
			currentIndex = i;
	}
	mIdCombo->setCurrentIndex(currentIndex);
}

std::vector<COORDINATE_SYSTEM> SpaceEditWidget::getAvailableSpaceIds()
{
	std::vector<COORDINATE_SYSTEM> retval;
	std::vector<Space> range = mData->getValueRange();

	for (unsigned i=0; i<range.size(); ++i)
	{
		if (!count(retval.begin(), retval.end(), range[i].mId))
			retval.push_back(range[i].mId);
	}

	return retval;
}

void SpaceEditWidget::rebuildRefCombobox()
{
	mRefCombo->clear();

	Space currentValue = mData->getValue();
	QStringList refs = this->getAvailableSpaceRefs(currentValue.mId);

	int currentIndex = -1;
	for (int i = 0; i < refs.size(); ++i)
	{
		QString ref = refs[i];
		QString name = mData->convertRefObjectInternal2Display(ref);
		mRefCombo->addItem(name);
		mRefCombo->setItemData(i, ref);
		if (ref == currentValue.mRefObject)
			currentIndex = i;
	}
	mRefCombo->setCurrentIndex(currentIndex);

	// Cannot set visibility inside a paint event.
	// Instead do it by placing a call in the queue.
	this->setRefComboVisibilityQueued();
}

void SpaceEditWidget::setRefComboVisibilityQueued()
{
	bool show = mRefCombo->count();

	if (mRefCombo->isVisible()==show)
		return;

	if (show)
		QMetaObject::invokeMethod(mRefCombo, "show", Qt::QueuedConnection);
	else
		QMetaObject::invokeMethod(mRefCombo, "hide", Qt::QueuedConnection);
}

QStringList SpaceEditWidget::getAvailableSpaceRefs(COORDINATE_SYSTEM id)
{
	QStringList retval;
	std::vector<Space> range = mData->getValueRange();

	for (unsigned i=0; i<range.size(); ++i)
	{
		if (range[i].mId == id)
			retval.push_back(range[i].mRefObject);
	}
	retval.removeDuplicates();
	retval.removeAll("");

	return retval;
}

void SpaceEditWidget::prePaintEvent()
{
	mRefCombo->blockSignals(true);
	mIdCombo->blockSignals(true);
	mIdCombo->clear();
	mRefCombo->clear();

	this->setEnabled(mData->getEnabled());
	mLabel->setEnabled(mData->getEnabled());
	mIdCombo->setEnabled(mData->getEnabled());
	mRefCombo->setEnabled(mData->getEnabled());

	this->rebuildIdCombobox();
	this->rebuildRefCombobox();

	mIdCombo->setToolTip(QString("%1\nSet space type").arg(mData->getHelp()));
	mRefCombo->setToolTip(QString("%1\nSet space identifier").arg(mData->getHelp()));
	mLabel->setToolTip(mData->getHelp());

	mRefCombo->blockSignals(false);
	mIdCombo->blockSignals(false);
}

} // namespace cx
