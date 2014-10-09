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
#include "cxPatientModelService.h"

namespace cx
{

TransferFunction2DOpacityWidget::TransferFunction2DOpacityWidget(PatientModelServicePtr patientModelService, QWidget* parent) :
  BaseWidget(parent, "TransferFunction2DOpacityWidget", "2D Opacity"),
  mPatientModelService(patientModelService)
{
  QVBoxLayout* layout = new QVBoxLayout(this);

  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(patientModelService, this);
  mTransferFunctionAlphaWidget->setReadOnly(true);

  mDataAlpha.reset(new DoubleDataAdapterImageTFDataAlpha);
  mDataLLR.reset(new DoubleDataAdapterImageTFDataLLR);

  mActiveImageProxy = ActiveImageProxy::New(patientModelService);
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);

  layout->addWidget(mTransferFunctionAlphaWidget);

  QGridLayout* gridLayout = new QGridLayout;
  layout->addLayout(gridLayout);
  new SliderGroupWidget(this, mDataAlpha,  gridLayout, 2);
  new SliderGroupWidget(this, mDataLLR,    gridLayout, 3);

  this->setLayout(layout);
}

QString TransferFunction2DOpacityWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>2D opacity transfer function.</h3>"
    "<p>Lets you set a 2D opacity transfer function.</p>"
    "<p><i></i></p>"
    "</html>";
}

void TransferFunction2DOpacityWidget::activeImageChangedSlot()
{
  ImagePtr image = mPatientModelService->getActiveImage();
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
