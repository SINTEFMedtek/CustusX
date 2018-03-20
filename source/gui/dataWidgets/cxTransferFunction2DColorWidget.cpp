/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTransferFunction2DColorWidget.h"
#include "cxDoubleWidgets.h"
#include "cxImageLUT2D.h"
#include "cxActiveData.h"

namespace cx
{

TransferFunction2DColorWidget::TransferFunction2DColorWidget(ActiveDataPtr activeData, QWidget* parent) :
  BaseWidget(parent, "transfer_function_2d_color_widget", "2D Color"),
  mActiveData(activeData)
{
	this->setToolTip("Set a 2D color transfer function");
  QVBoxLayout* layout = new QVBoxLayout(this);

  mTransferFunctionColorWidget = new TransferFunctionColorWidget(mActiveData, this);
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(mActiveData, this);
  mTransferFunctionAlphaWidget->setReadOnly(true);

  mDataWindow.reset(new DoublePropertyImageTFDataWindow);
  mDataLevel.reset(new DoublePropertyImageTFDataLevel);

  mActiveImageProxy = ActiveImageProxy::New(mActiveData);
  connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &TransferFunction2DColorWidget::activeImageChangedSlot);
  connect(mActiveData.get(), &ActiveData::activeDataChanged, this, &TransferFunction2DColorWidget::activeImageChangedSlot);

  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);
  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);

  layout->addWidget(mTransferFunctionAlphaWidget);

  layout->addWidget(mTransferFunctionColorWidget);

  QGridLayout* gridLayout = new QGridLayout;
  layout->addLayout(gridLayout);
  new SliderGroupWidget(this, mDataWindow, gridLayout, 0);
  new SliderGroupWidget(this, mDataLevel,  gridLayout, 1);

  this->setLayout(layout);
  this->activeImageChangedSlot();
}

TransferFunction2DColorWidget::~TransferFunction2DColorWidget()
{
	disconnect(mActiveData.get(), &ActiveData::activeDataChanged, this, &TransferFunction2DColorWidget::activeImageChangedSlot);
}

void TransferFunction2DColorWidget::activeImageChangedSlot()
{
	ImagePtr image = mActiveData->getDerivedActiveImage();

  ImageTFDataPtr tf;
  if (image)
    tf = image->getLookupTable2D();
  else
    image.reset();

  mTransferFunctionColorWidget->setData(image, tf);
  mTransferFunctionAlphaWidget->setData(image, tf);

  mDataWindow->setImageTFData(tf, image);
  mDataLevel->setImageTFData(tf, image);
}


}//end namespace cx
