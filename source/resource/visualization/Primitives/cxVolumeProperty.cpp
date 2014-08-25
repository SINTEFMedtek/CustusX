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

#include "cxVolumeProperty.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>

#include "cxImage.h"
#include "cxImageTF3D.h"


namespace cx
{

VolumeProperty::VolumeProperty() : QObject(),
	mOpacityTransferFunction(vtkPiecewiseFunctionPtr::New()),
	mColorTransferFunction(vtkColorTransferFunctionPtr::New()),
	mVolumeProperty(vtkVolumePropertyPtr::New())
{
	double maxVal = 255;
	mOpacityTransferFunction->AddPoint(0.0, 0.0);
	mOpacityTransferFunction->AddPoint(maxVal, 1.0);

	mColorTransferFunction->SetColorSpaceToRGB();
	mColorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	mColorTransferFunction->AddRGBPoint(maxVal, 1.0, 1.0, 1.0);

	mVolumeProperty->SetColor(mColorTransferFunction);
	mVolumeProperty->SetScalarOpacity(mOpacityTransferFunction);
	mVolumeProperty->SetInterpolationTypeToLinear();

	// from snw
	mVolumeProperty->ShadeOff();
	mVolumeProperty->SetAmbient ( 0.2 );
	mVolumeProperty->SetDiffuse ( 0.9 );
	mVolumeProperty->SetSpecular ( 0.3 );
	mVolumeProperty->SetSpecularPower ( 15.0 );
	mVolumeProperty->SetScalarOpacityUnitDistance(0.8919);
}

vtkVolumePropertyPtr VolumeProperty::getVolumeProperty()
{
	return mVolumeProperty;
}

void VolumeProperty::setImage(ImagePtr image)
{
	if (image==mImage)
	{
		return;
	}

	if (mImage)
	{
		disconnect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(transferFunctionsChangedSlot()));
		disconnect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
	}

	mImage = image;

	if (mImage)
	{
		connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(transferFunctionsChangedSlot()));
		connect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
	}

	this->transferFunctionsChangedSlot();
}

void VolumeProperty::transferFunctionsChangedSlot()
{
	if (!mImage)
		return;

	mVolumeProperty->SetColor(mImage->getTransferFunctions3D()->getColorTF());
	mVolumeProperty->SetScalarOpacity(mImage->getTransferFunctions3D()->getOpacityTF());
	mVolumeProperty->SetShade(mImage->getShadingOn());

	mVolumeProperty->SetAmbient(mImage->getShadingAmbient());
	mVolumeProperty->SetDiffuse(mImage->getShadingDiffuse());
	mVolumeProperty->SetSpecular(mImage->getShadingSpecular());
	mVolumeProperty->SetSpecularPower(mImage->getShadingSpecularPower());

	mVolumeProperty->SetInterpolationType(mImage->getInterpolationType());
}

} // namespace cx


