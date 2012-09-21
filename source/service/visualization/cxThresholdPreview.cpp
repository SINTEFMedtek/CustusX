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
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscVolumetricRep.h"
#include "cxRepManager.h"

namespace cx
{

ThresholdPreview::ThresholdPreview()
{
	//Timer for removing segmentation preview coloring if widget is not visible
	mRemoveTimer = new QTimer(this);
	connect(mRemoveTimer, SIGNAL(timeout()), this, SLOT(removeIfNotVisibleSlot()));
}

void ThresholdPreview::removeIfNotVisibleSlot()
{
	//Revert to original transfer functions when the widget is no longer visible
	if (mFromWidget->visibleRegion().isEmpty())
	{
		mRemoveTimer->stop();
		this->revertTransferFunctions();
	}
}

void ThresholdPreview::revertTransferFunctions()
{
	if (!mModifiedImage)
		return;

	mModifiedImage->resetTransferFunction(mTF3D_original, mTF2D_original);
	mModifiedImage->setShadingOn(mShadingOn_original);

	//Go back to VTK linear interpolation
	ssc::VolumetricRepPtr volumeRep = RepManager::getInstance()->getVolumetricRep(mModifiedImage);
	if(volumeRep)
		volumeRep->getVtkVolume()->GetProperty()->SetInterpolationTypeToLinear();
	else
		ssc::messageManager()->sendError("ThresholdPreview::revertTransferFunctions() can not find VolumetricRep");

	mTF3D_original.reset();
	mTF2D_original.reset();
	mModifiedImage.reset();

}

/**
 * Apply the preview transfer function
 *
 * A timer is added that will the remove the preview when the calling widget is no longer visible.
 *
 * \param fromWidget The calling widget
 * \param image The image to modify the transfer function of
 * \param setValue The threshold value to be used
 */
void ThresholdPreview::setPreview(QWidget* fromWidget, ssc::ImagePtr image, double setValue)
{
	if (!image)
		return;
	if (fromWidget->visibleRegion().isEmpty())
		return; // Don't do anything if the widget isn't visible

	//Revert value from old widget if a new one is trying to use setPreview()
	if (mFromWidget != fromWidget)
		removePreview(mFromWidget);
	mFromWidget = fromWidget;

	if (!mModifiedImage)
	{
		mModifiedImage = image;
		mTF3D_original = image->getTransferFunctions3D()->createCopy(image->getBaseVtkImageData());
		mTF2D_original = image->getLookupTable2D()->createCopy(image->getBaseVtkImageData());
		mShadingOn_original = image->getShadingOn();
	}
	image->resetTransferFunctions();
	ssc::ImageTF3DPtr tf3D = image->getTransferFunctions3D();
	tf3D->removeInitAlphaPoint();
	tf3D->addAlphaPoint(setValue - 1, 0);
	tf3D->addAlphaPoint(setValue, image->getMaxAlphaValue());
	tf3D->addColorPoint(setValue, Qt::green);
	tf3D->addColorPoint(image->getMax(), Qt::green);
	image->setShadingOn(true);

	ssc::ImageLUT2DPtr lut2D = image->getLookupTable2D();
	lut2D->setFullRangeWinLevel();
	lut2D->addColorPoint(setValue, Qt::green);
	lut2D->addColorPoint(image->getMax(), Qt::green);
	lut2D->setLLR(setValue);

	//Remove VTK linear interpolation
	ssc::VolumetricRepPtr volumeRep = RepManager::getInstance()->getVolumetricRep(image);
	if(volumeRep)
		volumeRep->getVtkVolume()->GetProperty()->SetInterpolationTypeToNearest();
	else
		ssc::messageManager()->sendError("ThresholdPreview::setPreview() can not find VolumetricRep");

	//Start timer that reverts transfer functions when widget is no longer visible
	mRemoveTimer->start(500);
}

void ThresholdPreview::removePreview(QWidget* fromWidget)
{
	this->revertTransferFunctions();
}
}
