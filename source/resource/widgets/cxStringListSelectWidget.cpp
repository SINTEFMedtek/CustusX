/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStringListSelectWidget.h"

#include <iostream>
#include "cxTypeConversions.h"
#include "cxHelperWidgets.h"
#include "cxLogger.h"
#include <QWidgetAction>
#include <QCheckBox>
#include <QMenu>
#include "boost/bind.hpp"
#include "cxStringListProperty.h"
#include "cxData.h"

namespace cx
{

StringListSelectWidget::StringListSelectWidget(QWidget* parent, StringListPropertyPtr property,
	QGridLayout* gridLayout, int row) :
	BaseWidget(parent, "StringListSelectWidget", "StringListSelectWidget"),
	mData(property)
{
	connect(mData.get(), &Property::changed, this, &StringListSelectWidget::setModified);

	this->setEnabled(mData->getEnabled());

	mLabel = new QLabel(this);
	mLabel->setText(mData->getDisplayName());

	mMenu = new QMenu(this);
	mButton = new CXSmallToolButton();
	mButton->setIcon(QIcon(":icons/open_icon_library/go-down-4.png"));
	mButton->setPopupMode(QToolButton::InstantPopup);
	mButton->setMenu(mMenu);

	if (gridLayout) // add to input gridlayout
	{
		gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);
		gridLayout->addWidget(mButton, row, 1);
	}
	else // add directly to this
	{
		mTopLayout = new QHBoxLayout;
		mTopLayout->setMargin(0);
		this->setLayout(mTopLayout);

		mTopLayout->addWidget(mLabel);
		mTopLayout->addWidget(mButton, 1);
	}

	this->setModified();
}

void StringListSelectWidget::setIcon(QIcon icon)
{
	mButton->setIcon(icon);
}

void StringListSelectWidget::onCheckToggled(QString nodeType, bool value)
{
	QStringList data = mData->getValue();

	if (value)
		data.append(nodeType);
	else
		data.removeAll(nodeType);

	mData->setValue(data);
}

void StringListSelectWidget::showLabel(bool on)
{
	mLabel->setVisible(on);
}

void StringListSelectWidget::prePaintEvent()
{
	this->setEnabled(mData->getEnabled());
	mLabel->setEnabled(mData->getEnabled());
	mButton->setEnabled(mData->getEnabled());

	QStringList value = mData->getValue();
	QStringList range = mData->getValueRange();

	if (mCachedRange!=range)
	{
		mCachedRange = range;
		mCheckBoxes.clear();
		mMenu->clear();

		for (int i=0; i<range.size(); ++i)
		{
			QString uid = range[i];
			QString name = mData->convertInternal2Display(uid);
			mCheckBoxes.push_back(new QCheckBox(name, mMenu));
			boost::function<void(bool)> func = boost::bind(&StringListSelectWidget::onCheckToggled, this, uid, _1);
			connect(mCheckBoxes[i], &QCheckBox::toggled, func);
			QWidgetAction *checkableAction = new QWidgetAction(mMenu);
			checkableAction->setDefaultWidget(mCheckBoxes[i]);
			mMenu->addAction(checkableAction);
		}
	}

	for (int i=0; i<range.size(); ++i)
	{
		mCheckBoxes[i]->setChecked(value.contains(range[i]));
	}

	mButton->setToolTip(mData->getHelp());
	mLabel->setToolTip(mData->getHelp());
}

} // namespace cx

