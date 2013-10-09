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

#include "cxVolumeProperty.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>

#include "sscImage.h"
#include "sscImageTF3D.h"


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


