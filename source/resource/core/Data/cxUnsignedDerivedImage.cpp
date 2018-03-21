/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxUnsignedDerivedImage.h"

#include <vtkUnsignedCharArray.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>

#include "cxImage.h"

#include <vtkImageResample.h>
#include <vtkImageClip.h>
#include "vtkImageShiftScale.h"

#include "cxImage.h"
#include "cxUtilHelpers.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxRegistrationTransform.h"


#include "cxCoordinateSystemHelpers.h"
#include "cxLogger.h"

typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;

namespace cx
{

ImagePtr UnsignedDerivedImage::create(ImagePtr base)
{
    boost::shared_ptr<UnsignedDerivedImage> retval;
    retval.reset(new UnsignedDerivedImage(base));
    return retval;
}

UnsignedDerivedImage::UnsignedDerivedImage(ImagePtr base) : Image(base->getUid()+"_u", vtkImageDataPtr(), base->getName())
{
    this->mBase = base;

    // redirected signals:
    connect(base.get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
    connect(base.get(), SIGNAL(propertiesChanged()), this, SIGNAL(propertiesChanged()));
    connect(base.get(), SIGNAL(clipPlanesChanged()), this, SIGNAL(clipPlanesChanged()));
    connect(base.get(), SIGNAL(cropBoxChanged()), this, SIGNAL(cropBoxChanged()));

    // override signals:
    connect(base.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(unsignedTransferFunctionsChangedSlot()));
    connect(base.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(unsignedImageChangedSlot()));

	connect(this, SIGNAL(transferFunctionsChanged()), this, SLOT(testSlot()));
	this->unsignedImageChangedSlot();
    this->unsignedTransferFunctionsChangedSlot();
}

void UnsignedDerivedImage::testSlot()
{
}

void UnsignedDerivedImage::unsignedTransferFunctionsChangedSlot()
{
	ImagePtr base = mBase.lock();
    if (!base)
        return;


	// this is a slow operation, triggered every time the mBase is changed. Take care.

    int shift = this->findShift();

	ImageTF3DPtr TF3D = base->getTransferFunctions3D()->createCopy();
	ImageLUT2DPtr LUT2D = base->getLookupTable2D()->createCopy();
	TF3D->shift(shift);
	LUT2D->shift(shift);
	this->setLookupTable2D(LUT2D);
	this->setTransferFunctions3D(TF3D);
}

void UnsignedDerivedImage::unsignedImageChangedSlot()
{
    this->setVtkImageData(this->convertImage());
}

int UnsignedDerivedImage::findShift()
{
    ImagePtr base = mBase.lock();
    if (!base)
        return 0;
    vtkImageDataPtr input = base->getBaseVtkImageData();

    if (input->GetScalarTypeMin() >= 0)
        return 0;

    // start by shifting up to zero
    int shift = -input->GetScalarRange()[0];
    // if CT: always shift by 1024 (houndsfield units definition)
    if (base->getModality().contains("CT", Qt::CaseInsensitive))
        shift = 1024;
    return shift;
}

vtkImageDataPtr UnsignedDerivedImage::convertImage()
{
    vtkImageDataPtr retval;

    ImagePtr base = mBase.lock();
    if (!base)
        return retval;

    int shift = this->findShift();
    vtkImageDataPtr input = base->getBaseVtkImageData();

    vtkImageShiftScalePtr cast = vtkImageShiftScalePtr::New();
	cast->SetInputData(input);
    cast->ClampOverflowOn();

    cast->SetShift(shift);

    // total intensity range of voxels:
    double range = input->GetScalarRange()[1] - input->GetScalarRange()[0];

    // to to fit within smallest type
    if (range <= VTK_UNSIGNED_SHORT_MAX-VTK_UNSIGNED_SHORT_MIN)
        cast->SetOutputScalarType(VTK_UNSIGNED_SHORT);
    else if (range <= VTK_UNSIGNED_INT_MAX-VTK_UNSIGNED_INT_MIN)
        cast->SetOutputScalarType(VTK_UNSIGNED_INT);
//	else if (range <= VTK_UNSIGNED_LONG_MAX-VTK_UNSIGNED_LONG_MIN) // not supported by vtk - it seems (crash in rendering)
//		cast->SetOutputScalarType(VTK_UNSIGNED_LONG);
    else
        cast->SetOutputScalarType(VTK_UNSIGNED_INT);

    cast->Update();
//		if (verbose)
      report(QString("Converting image %1 from %2 to %3").arg(this->getName()).arg(input->GetScalarTypeAsString()).arg(cast->GetOutput()->GetScalarTypeAsString()));
    retval = cast->GetOutput();
    return retval;
}

CoordinateSystem UnsignedDerivedImage::getCoordinateSystem()
{
	CALL_IN_WEAK_PTR(mBase, getCoordinateSystem, CoordinateSystem(csCOUNT));
}

}
