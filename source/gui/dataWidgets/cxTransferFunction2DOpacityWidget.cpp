/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTransferFunction2DOpacityWidget.h"
#include "cxDoubleWidgets.h"
#include "cxImageLUT2D.h"
#include "cxActiveData.h"

namespace cx
{

TransferFunction2DOpacityWidget::TransferFunction2DOpacityWidget(ActiveDataPtr activeData, QWidget* parent) :
	BaseWidget(parent, "transfer_function_2d_opacity_widget", "2D Opacity"),
	mActiveData(activeData)
{
	this->setToolTip("Set a 2D opacity transfer function");
  QVBoxLayout* layout = new QVBoxLayout(this);

  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(mActiveData, this);
  mTransferFunctionAlphaWidget->setReadOnly(true);

  mDataAlpha.reset(new DoublePropertyImageTFDataAlpha);
  mDataLLR.reset(new DoublePropertyImageTFDataLLR);

  mActiveImageProxy = ActiveImageProxy::New(mActiveData);
  connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &TransferFunction2DOpacityWidget::activeImageChangedSlot);
  connect(mActiveData.get(), &ActiveData::activeDataChanged, this, &TransferFunction2DOpacityWidget::activeImageChangedSlot);

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);

  layout->addWidget(mTransferFunctionAlphaWidget);

  QGridLayout* gridLayout = new QGridLayout;
  layout->addLayout(gridLayout);
  new SliderGroupWidget(this, mDataAlpha,  gridLayout, 2);
  new SliderGroupWidget(this, mDataLLR,    gridLayout, 3);

  this->setLayout(layout);
  this->activeImageChangedSlot();
}

TransferFunction2DOpacityWidget::~TransferFunction2DOpacityWidget()
{
	disconnect(mActiveData.get(), &ActiveData::activeDataChanged, this, &TransferFunction2DOpacityWidget::activeImageChangedSlot);
}

void TransferFunction2DOpacityWidget::activeImageChangedSlot()
{
	ImagePtr image = mActiveData->getDerivedActiveImage();
  ImageTFDataPtr tf;
  if (image)
    tf = image->getLookupTable2D();
  else
    image.reset();

  mTransferFunctionAlphaWidget->setData(image, tf);

  mDataAlpha->setImageTFData(tf, image);
  mDataLLR->setImageTFData(tf, image);
}


}//end namespace cx
