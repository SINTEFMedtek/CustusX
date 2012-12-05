// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "sscColorSelectWidget.h"
#include "sscHelperWidgets.h"

namespace ssc
{

ColorSelectWidget::ColorSelectWidget(QWidget* parent, ssc::ColorDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) :
    OptimizedUpdateWidget(parent)
{
    mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

    mLabel = new QLabel(this);
    mLabel->setText(dataInterface->getValueName());

    mColorButton = new cx::ColorSelectButton("");
    connect(mColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(valueChanged(QColor)));

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

    mColorButton->setColor(mData->getValue());
    mColorButton->setToolTip(mData->getHelp());

    mColorButton->blockSignals(false);
}


} // namespace ssc

