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

#include "cxTransferFunction2DColorWidget.h"
#include "cxDoubleWidgets.h"
#include "cxImageLUT2D.h"
#include "cxActiveData.h"

namespace cx
{

TransferFunction2DColorWidget::TransferFunction2DColorWidget(ActiveDataPtr activeData, QWidget* parent) :
  BaseWidget(parent, "TransferFunction2DColorWidget", "2D Color"),
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
