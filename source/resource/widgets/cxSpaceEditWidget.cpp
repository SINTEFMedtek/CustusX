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

#include "cxSpaceEditWidget.h"
#include <iostream>
#include "cxTypeConversions.h"
#include "cxHelperWidgets.h"
#include "cxLogger.h"

namespace cx
{

SpaceEditWidget::SpaceEditWidget(QWidget* parent, StringDataAdapterPtr dataInterface,
	QGridLayout* gridLayout, int row) :
	BaseWidget(parent, "SpaceEditWidget", "SpaceEditWidget")
{
	SSC_ASSERT(dataInterface->getAllowOnlyValuesInRange()==true);

	this->setEnabled(dataInterface->getEnabled());

	mData = dataInterface;
	connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

	mLabel = new QLabel(this);
	mLabel->setText(mData->getDisplayName());

	mIdCombo = new QComboBox(this);
	connect(mIdCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged()));
//	connect(mIdCombo, &QComboBox::currentIndexChanged, this, &SpaceEditWidget::comboIndexChanged);

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

QString SpaceEditWidget::defaultWhatsThis() const
{
	return "<html></html>";
}

void SpaceEditWidget::comboIndexChanged()
{
	QString selected = this->mergeSpace(mIdCombo->currentData().toString(), mRefCombo->currentData().toString());
	mData->setValue(selected);
}

void SpaceEditWidget::showLabel(bool on)
{
	mLabel->setVisible(on);
}

QString SpaceEditWidget::getNameForUid(QString uid, int index, std::vector<SpaceData> allSpaces)
{
	for (int i=0; i<allSpaces.size(); ++i)
	{
		if (allSpaces[i].uidlist()[index] == uid)
			return allSpaces[i].namelist()[index];
	}
	return "";
}

void SpaceEditWidget::rebuildCombobox(QComboBox* widget, QStringList uids, int index, std::vector<SpaceData> allSpaces, QString currentUid)
{
	widget->blockSignals(true);
	widget->clear();

	int currentIndex = -1;
	for (int i = 0; i < uids.size(); ++i)
	{
		QString uid = uids[i];
		QString name = this->getNameForUid(uid, index, allSpaces);

		std::cout << "add item " << uid << " -- " << name << std::endl;

		widget->addItem(name);
		widget->setItemData(i, uid);
		if (uid == currentUid)
			currentIndex = i;
	}
	widget->setCurrentIndex(currentIndex);
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

	QString currentValue = mData->getValue();
	std::cout << "currentValue " << currentValue << std::endl;
	QStringList currentuidlist = this->splitSpace(currentValue); // id+ref for current
	QStringList range = mData->getValueRange(); // uid for all

	std::vector<SpaceData> allSpaces;
	QStringList allIds;
	QStringList allRefs;

	for (int i=0; i<range.size(); ++i)
	{
		SpaceData space(range[i], mData->convertInternal2Display(range[i]));
		std::cout << "val ["<<i<<"] " << range[i] << " -- " <<mData->convertInternal2Display(range[i]) << std::endl;
		allSpaces.push_back(space);
		allIds.push_back(space.uidlist()[0]);
		if (space.uidlist()[0] == currentuidlist[0])
			allRefs.push_back(space.uidlist()[1]);
	}
	allIds.removeDuplicates();
	allRefs.removeDuplicates();
	allRefs.removeAll("");

	std::cout << "allIds " << allIds.join(" -- ") << std::endl;
	std::cout << "allRefs " << allRefs.join(" -- ") << std::endl;
	std::cout << "currentuidlist  " << currentuidlist.join(" -- ") << std::endl;

	mRefCombo->blockSignals(true);
	mIdCombo->blockSignals(true);

	std::cout << "=== ids " << std::endl;
	this->rebuildCombobox(mIdCombo, allIds, 0, allSpaces, currentuidlist[0]);
	std::cout << "=== refs " << std::endl;
	this->rebuildCombobox(mRefCombo, allRefs, 1, allSpaces, currentuidlist[1]);
	std::cout << "=== end " << std::endl;

//	int refIndex = -1;
//	for (int i = 0; i < allRefs.size(); ++i)
//	{
//		QString uid = allRefs[i];
//		QString name = this->getNameForUid(uid, 1, allSpaces);

//		mIdCombo->addItem(name);
//		mIdCombo->setItemData(i, uid);
//		if (uid == currentuidlist[1])
//			refIndex = i;
//	}
//	mIdCombo->setCurrentIndex(refIndex);

	mIdCombo->setToolTip(mData->getHelp());
	mRefCombo->setToolTip(mData->getHelp());
	mLabel->setToolTip(mData->getHelp());

	mRefCombo->blockSignals(false);
	mIdCombo->blockSignals(false);
}

QStringList SpaceEditWidget::splitSpace(QString name) const
{
	QStringList values = name.split("/");
	if (values.size()==1)
		values << "";
	return values;
}

QString SpaceEditWidget::mergeSpace(QString a, QString b) const
{
	std::cout << "!!! set " << a << " -- " << b << std::endl;

	QStringList values;
	values << a << b;
	values.removeAll("");
	QString selected = values.join("/");
	std::cout << "!!! setted " << selected << std::endl;
	return selected;
}

} // namespace cx
