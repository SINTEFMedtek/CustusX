/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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


