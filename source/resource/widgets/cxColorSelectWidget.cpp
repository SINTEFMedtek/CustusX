/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxColorSelectWidget.h"
#include "cxHelperWidgets.h"

namespace cx
{

ColorSelectWidget::ColorSelectWidget(QWidget* parent, ColorPropertyBasePtr dataInterface, QGridLayout* gridLayout, int row) :
    OptimizedUpdateWidget(parent)
{
	this->setEnabled(dataInterface->getEnabled());

    mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

    mLabel = new QLabel(this);
    mLabel->setText(dataInterface->getDisplayName());

    mColorButton = new cx::ColorSelectButton("");

	// emit as queued signal because of crash issues related to handling the signal
	// in the modal loop of the color select dialog of cx:ColorSelectButton.
	connect(mColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(valueChanged(QColor)), Qt::QueuedConnection);

    if (gridLayout) // add to input gridlayout
    {
        gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);
        gridLayout->addWidget(mColorButton, row, 1);
    }
    else // add directly to this
    {
        QHBoxLayout* topLayout = new QHBoxLayout;
        topLayout->setMargin(0);
        this->setLayout(topLayout);

        topLayout->addWidget(mLabel);
        topLayout->addWidget(mColorButton, 1);
    }

    this->setModified();
}

void ColorSelectWidget::valueChanged(QColor val)
{
    if (val == mData->getValue())
        return;
    mData->setValue(val);
}

void ColorSelectWidget::prePaintEvent()
{
    mColorButton->blockSignals(true);

    this->setEnabled(mData->getEnabled());

    mColorButton->setColor(mData->getValue());
    mColorButton->setToolTip(mData->getHelp());
	mLabel->setToolTip(mData->getHelp());

    mColorButton->blockSignals(false);
}


} // namespace cx

