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

#include "cxThresholdPreview.h"
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <QWidget>
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxVolumetricRep.h"
#include "cxRepManager.h"
#include <QTimer>
#include "cxImage.h"
#include "cxReporter.h"

namespace cx
{


WidgetObscuredListener::WidgetObscuredListener(QWidget *listenedTo) : mWidget(listenedTo)
{
    mRemoveTimer = new QTimer(this);
    connect(mRemoveTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
    mRemoveTimer->start(500);

    mObscuredAtLastCheck = this->isObscured();
}

bool WidgetObscuredListener::isObscured() const
{
    return mWidget->visibleRegion().isEmpty();
}

void WidgetObscuredListener::timeoutSlot()
{
    if (mObscuredAtLastCheck == this->isObscured())
        return;

    mObscuredAtLastCheck = this->isObscured();
    emit obscured(mObscuredAtLastCheck);
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ThresholdPreview::ThresholdPreview() :
	mShadingOn_original(false)
{
}

void ThresholdPreview::revertTransferFunctions()
{
    if (!mModifiedImage)
        return;

    mModifiedImage->setLookupTable2D(mTF2D_original);
    mModifiedImage->setTransferFunctions3D(mTF3D_original);
    mModifiedImage->setShadingOn(mShadingOn_original);

	mModifiedImage->setInterpolationTypeToLinear();
//    //Go back to VTK linear interpolation
//	VolumetricBaseRepPtr volumeRep = RepManager::getInstance()->getVolumetricRep(mModifiedImage);
//    if(volumeRep)
//        volumeRep->getVtkVolume()->GetProperty()->SetInterpolationTypeToLinear();
//    else
//        reportError("ThresholdPreview::revertTransferFunctions() can not find VolumetricRep");

    mTF3D_original.reset();
    mTF2D_original.reset();
    mModifiedImage.reset();
}

/**
 * Apply the preview transfer function
 *
 * A timer is added that will the remove the preview when the calling widget is no longer visible.
 *
 * \param image The image to modify the transfer function of
 * \param setValue The threshold value to be used
 */
void ThresholdPreview::setPreview(ImagePtr image, double setValue)
{
    if (!image)
        return;

    if (image!=mModifiedImage)
    {
        this->removePreview();
    }

//    std::cout << "ThresholdPreview::setPreview " << image->getName() << " - " << setValue << std::endl;

    if (!mModifiedImage)
    {
        mModifiedImage = image;
		mTF3D_original = image->getTransferFunctions3D()->createCopy();
		mTF2D_original = image->getLookupTable2D()->createCopy();
        mShadingOn_original = image->getShadingOn();
    }
    image->resetTransferFunctions();
    ImageTF3DPtr tf3D = image->getTransferFunctions3D();

	ColorMap colors;
	colors[setValue] = Qt::green;
	colors[image->getMax()] = Qt::green;
	tf3D->resetColor(colors);

	IntIntMap opacity;
	opacity[setValue - 1] = 0;
	opacity[setValue] = image->getMaxAlphaValue();
	tf3D->resetAlpha(opacity);


//	tf3D->removeInitAlphaPoint();
//    tf3D->addAlphaPoint(setValue - 1, 0);
//    tf3D->addAlphaPoint(setValue, image->getMaxAlphaValue());
//    tf3D->addColorPoint(setValue, Qt::green);
//    tf3D->addColorPoint(image->getMax(), Qt::green);
    image->setShadingOn(true);

    ImageLUT2DPtr lut2D = image->getLookupTable2D();
	colors.clear();
	colors[setValue] = Qt::green;
	colors[image->getMax()] = Qt::green;
	lut2D->resetColor(colors);
//	lut2D->setFullRangeWinLevel(source->getVtkImageData());
//    lut2D->addColorPoint(setValue, Qt::green);
//    lut2D->addColorPoint(image->getMax(), Qt::green);
    lut2D->setLLR(setValue);


    //Remove VTK linear interpolation
	mModifiedImage->setInterpolationTypeToNearest();
//	VolumetricBaseRepPtr volumeRep = RepManager::getInstance()->getVolumetricRep(image);
//    if(volumeRep)
//        volumeRep->getVtkVolume()->GetProperty()->SetInterpolationTypeToNearest();
//    else
//        reportError("ThresholdPreview::setPreview() can not find VolumetricRep");
}

void ThresholdPreview::removePreview()
{
//    std::cout << "ThresholdPreview::removePreview " << std::endl;
    this->revertTransferFunctions();
}

}
