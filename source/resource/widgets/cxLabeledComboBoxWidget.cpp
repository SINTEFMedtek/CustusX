/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	this->setEnabled(mData->getEnabled());
	mLabel->setEnabled(mData->getEnabled());
	mCombo->setEnabled(mData->getEnabled());

	QString currentValue = mData->getValue();
	QStringList range = mData->getValueRange();
	if (range.size()!=mCombo->count())
	{
		mCombo->clear();
		mCombo->addItems(range);
	}
	int currentIndex = -1;
	for (int i = 0; i < range.size(); ++i)
	{
		mCombo->setItemIcon(i, this->getIcon(range[i]));
		mCombo->setItemText(i, mData->convertInternal2Display(range[i]));
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
