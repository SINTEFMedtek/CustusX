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


#include "cxLabeledComboBoxWidget.h"
#include <iostream>
#include "cxTypeConversions.h"
#include "cxHelperWidgets.h"
#include "cxLogger.h"

#include "cxSelectDataStringPropertyBase.h"
#include "cxData.h"

namespace cx
{

LabeledComboBoxWidget::LabeledComboBoxWidget(QWidget* parent, StringPropertyBasePtr dataInterface,
    QGridLayout* gridLayout, int row) :
    BaseWidget(parent, "LabeledComboBoxWidget", "LabeledComboBoxWidget")
{
	CX_ASSERT(dataInterface->getAllowOnlyValuesInRange()==true);

	this->setEnabled(dataInterface->getEnabled());

	mData = dataInterface;
	connect(mData.get(), &Property::changed, this, &LabeledComboBoxWidget::setModified);

	mLabel = new QLabel(this);
	mLabel->setText(mData->getDisplayName());

	mCombo = new QComboBox(this);
	connect(mCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));

	if (gridLayout) // add to input gridlayout
	{
        gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);
		gridLayout->addWidget(mCombo, row, 1);
	}
	else // add directly to this
	{
        mTopLayout = new QHBoxLayout;
        mTopLayout->setMargin(0);
        this->setLayout(mTopLayout);

        mTopLayout->addWidget(mLabel);
        mTopLayout->addWidget(mCombo, 1);
	}

    this->setModified();
}

void LabeledComboBoxWidget::comboIndexChanged(int index)
{
	mData->setValue(mCombo->itemData(index).toString());
}

void LabeledComboBoxWidget::showLabel(bool on)
{
	mLabel->setVisible(on);
}

void LabeledComboBoxWidget::prePaintEvent()
{
	mCombo->blockSignals(true);
	mCombo->clear();

	this->setEnabled(mData->getEnabled());
	mLabel->setEnabled(mData->getEnabled());
	mCombo->setEnabled(mData->getEnabled());

	QString currentValue = mData->getValue();
	QStringList range = mData->getValueRange();
	int currentIndex = -1;
	for (int i = 0; i < range.size(); ++i)
	{
		QIcon icon = this->getIcon(range[i]);
		mCombo->addItem(icon, mData->convertInternal2Display(range[i]));
		mCombo->setItemData(i, range[i]);
		if (range[i] == currentValue)
			currentIndex = i;
	}
	mCombo->setCurrentIndex(currentIndex);

	mCombo->setToolTip(mData->getHelp());
	mLabel->setToolTip(mData->getHelp());
	mCombo->blockSignals(false);
}

QIcon LabeledComboBoxWidget::getIcon(QString uid)
{
	SelectDataStringPropertyBasePtr dataProperty = boost::dynamic_pointer_cast<SelectDataStringPropertyBase>(mData);
	if(!dataProperty)
		return QIcon();

	DataPtr data = dataProperty->getData(uid);
	if(!data)
		return QIcon();
	return data->getIcon();
}

} // namespace cx
