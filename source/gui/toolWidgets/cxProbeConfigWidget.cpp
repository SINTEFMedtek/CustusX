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

#include <cxProbeConfigWidget.h>
#include "sscLabeledComboBoxWidget.h"
#include "sscDoubleWidgets.h"
#include "cxToolDataAdapters.h"

namespace cx
{

ProbeConfigWidget::ProbeConfigWidget(QWidget* parent) : BaseWidget(parent, "ProbeConfigWidget", "Probe Configuration")
{
	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(new QLabel("Probe!!!!"));
	mActiveProbeConfig = ActiveProbeConfigurationStringDataAdapter::New();
	connect(mActiveProbeConfig.get(), SIGNAL(changed()), this, SLOT());
	topLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mActiveProbeConfig));

//	mBBWidget = new BoundingBoxWidget(this);
//	topLayout->addWidget(mBBWidget);
//	connect(mBBWidget, SIGNAL(changed()), this, SLOT(boxValuesChanged()));

}

ProbeConfigWidget::~ProbeConfigWidget()
{
}

QString ProbeConfigWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Probe Configuration</h3>"
      "<p>View and edit the probe configuration.</p>"
      "<p><i></i></p>"
      "</html>";
}

//void ProbeConfigWidget::boxValuesChanged()
//{
//	mInteractiveCropper->setBoundingBox(mBBWidget->getValue());
//}
//
//void ProbeConfigWidget::cropperChangedSlot()
//{
//	mUseCropperCheckBox->setChecked(mInteractiveCropper->getUseCropping());
//	mShowBoxCheckBox->setChecked(mInteractiveCropper->getShowBoxWidget());
//
//	mBBWidget->setValue(mInteractiveCropper->getBoundingBox(), mInteractiveCropper->getMaxBoundingBox());
//}



}
