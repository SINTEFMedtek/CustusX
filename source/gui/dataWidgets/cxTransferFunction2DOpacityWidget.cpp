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
