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
			QString text = range[i];
			mCheckBoxes.push_back(new QCheckBox(text, mMenu));
			boost::function<void(bool)> func = boost::bind(&StringListSelectWidget::onCheckToggled, this, text, _1);
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

