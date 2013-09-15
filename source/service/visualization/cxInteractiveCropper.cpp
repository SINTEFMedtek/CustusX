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

/*
 * cxInteractiveCropper.cpp
 *
 *  \date Aug 24, 2010
 *      \author christiana
 */
#include "cxInteractiveCropper.h"

#include "sscView.h"

#include <vector>
#include <vtkTransform.h>
//#include <vtkPolyData.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkVolumeMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include "sscTypeConversions.h"
#include "cxRepManager.h"
#include "sscDataManager.h"
#include <vtkBoxWidget2.h>
#include <vtkBoxWidget.h>
#include "sscBoundingBox3D.h"
#include "sscImage.h"
#include "sscTransform3D.h"
#include "sscVolumetricRep.h"
#include "cxActiveImageProxy.h"

namespace cx
{

class CropBoxCallback: public vtkCommand
{
public:
	CropBoxCallback() : mCropper(NULL)
	{
	}
	static CropBoxCallback* New()
	{
		return new CropBoxCallback;
	}
	void SetCropper(InteractiveCropper* cropper)
	{
		mCropper = cropper;
	}
	virtual void Execute(vtkObject* caller, unsigned long, void*)
	{
		DoubleBoundingBox3D bb_new = mCropper->getBoxWidgetSize();
		mCropper->setCroppingRegion(bb_new);
	}
	InteractiveCropper* mCropper;
};

class CropBoxEnableCallback: public vtkCommand
{
public:
	CropBoxEnableCallback() : mCropper(NULL), mValue(false)
	{
	}
	static CropBoxEnableCallback* New()
	{
		return new CropBoxEnableCallback;
	}
	void SetCropper(bool val, InteractiveCropper* cropper)
	{
		mValue = val;
		mCropper = cropper;
	}
	virtual void Execute(vtkObject* caller, unsigned long, void*)
	{
		DoubleBoundingBox3D bb_new = mCropper->getBoxWidgetSize();
		mCropper->boxWasShown(mValue);
	}
	bool mValue;
	InteractiveCropper* mCropper;
};

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


InteractiveCropper::InteractiveCropper()
{
	mActiveImageProxy = ActiveImageProxy::New();
	connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(imageChangedSlot()));
	connect(mActiveImageProxy.get(), SIGNAL(cropBoxChanged()), this, SLOT(imageCropChangedSlot()));
}

void InteractiveCropper::initialize()
{
	if (mBoxWidget) // already initialized
		return;

	mBoxWidget = vtkBoxWidgetPtr::New();
	mBoxWidget->RotationEnabledOff();

	double bb_hard[6] =
	{ -1, 1, -1, 1, -1, 1 };
	mBoxWidget->PlaceWidget(bb_hard);

	mCropBoxCallback = CropBoxCallbackPtr::New();
	mCropBoxCallback->SetCropper(this);
	mCropBoxEnableCallback = CropBoxEnableCallbackPtr::New();
	mCropBoxEnableCallback->SetCropper(true, this);
	mCropBoxDisableCallback = CropBoxEnableCallbackPtr::New();
	mCropBoxDisableCallback->SetCropper(false, this);

	mBoxWidget->SetInteractor(mView->getRenderWindow()->GetInteractor());

	mBoxWidget->SetEnabled(false);
}

void InteractiveCropper::setView(ViewWidget* view)
{
	mView = view;
	this->updateBoxWidgetInteractor();
}

void InteractiveCropper::updateBoxWidgetInteractor()
{
	if (!mView)
		return;

	this->initialize();

	if (this->getUseCropping())
	{
		mBoxWidget->SetInteractor(mView->getRenderWindow()->GetInteractor());
		mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCropBoxCallback);
		mBoxWidget->AddObserver(vtkCommand::EnableEvent, mCropBoxEnableCallback);
		mBoxWidget->AddObserver(vtkCommand::DisableEvent, mCropBoxDisableCallback);
	}
	else
	{
		mBoxWidget->RemoveObserver(vtkCommand::InteractionEvent);
		mBoxWidget->RemoveObserver(vtkCommand::EnableEvent);
		mBoxWidget->RemoveObserver(vtkCommand::DisableEvent);
	}
}

void InteractiveCropper::showBoxWidget(bool on)
{
	if (!mImage)
		return;
	if (!mBoxWidget)
		return;
	if (this->getShowBoxWidget() == on)
		return;

	//Turn on cropping if not on to save user from pressing two boxes
	if (!mImage->getCropping() && on)
		this->useCropping(true);

	mBoxWidget->SetEnabled(on);
	emit changed();
}

/** get current cropping box in ref coords
 */
DoubleBoundingBox3D InteractiveCropper::getBoundingBox()
{
	if (!mImage || !mBoxWidget)
		return DoubleBoundingBox3D(0,0,0,0,0,0);
	return mImage->getCroppingBox();
}

void InteractiveCropper::setBoundingBox(const DoubleBoundingBox3D& bb_d)
{
	this->setCroppingRegion(bb_d);
	this->setBoxWidgetSize(bb_d);
}

void InteractiveCropper::useCropping(bool on)
{
	if (this->getUseCropping() == on)
		return;

	if (!mImage)
		return;

	mImage->setCropping(on);
}

void InteractiveCropper::imageCropChangedSlot()
{
	if (!mImage)
		return;

	DoubleBoundingBox3D bb_d = this->getBoundingBox();
	this->setBoxWidgetSize(bb_d);
	this->updateBoxWidgetInteractor();

	if (!mImage->getCropping())
		this->showBoxWidget(false);

	emit changed();
}

void InteractiveCropper::resetBoundingBox()
{
	emit changed();
}

void InteractiveCropper::imageChangedSlot()
{
	mImage = dataManager()->getActiveImage();

	this->imageCropChangedSlot();
	emit changed();
}

bool InteractiveCropper::getUseCropping()
{
	if (!mImage)
		return false;
	return mImage->getCropping();
}

bool InteractiveCropper::getShowBoxWidget() const
{
	if (!mBoxWidget)
		return false;
	return mBoxWidget->GetEnabled();
}

std::vector<int> InteractiveCropper::getDimensions()
{
	std::vector<int> dimensions;
	if(!mImage)
		return dimensions;

	double spacing_x = 1;
	double spacing_y = 1;
	double spacing_z = 1;
	mImage->getBaseVtkImageData()->GetSpacing(spacing_x, spacing_y, spacing_z);

	DoubleBoundingBox3D bb = getBoxWidgetSize();
	int dim_x = (bb.begin()[1] - bb.begin()[0])/spacing_x + 1; //adding 1 because of some rounding errors, is there a better way to do this?
	int dim_y = (bb.begin()[3] - bb.begin()[2])/spacing_y + 1;
	int dim_z = (bb.begin()[5] - bb.begin()[4])/spacing_z + 1;
	dimensions.push_back(dim_x);
	dimensions.push_back(dim_y);
	dimensions.push_back(dim_z);

	return dimensions;
}

/** Set the box widget bounding box to the input box (given in data space)
 */
void InteractiveCropper::setBoxWidgetSize(const DoubleBoundingBox3D& bb_d)
{
	if (!mImage || !mBoxWidget)
		return;

	double bb_hard[6] =
	{ -0.5, 0.5, -0.5, 0.5, -0.5, 0.5 };
	DoubleBoundingBox3D bb_unit(bb_hard);
	Transform3D M = createTransformNormalize(bb_unit, bb_d);
	Transform3D rMd = mImage->get_rMd();
	M = rMd * M;

	vtkTransformPtr transform = vtkTransformPtr::New();
	transform->SetMatrix(M.getVtkMatrix());
	mBoxWidget->SetTransform(transform);
}

/** return the bow widget current size in data space
 */
DoubleBoundingBox3D InteractiveCropper::getBoxWidgetSize()
{
	if (!mImage || !mBoxWidget)
	{
		return DoubleBoundingBox3D::zero();
	}

	double bb_hard[6] =
	{ -0.5, 0.5, -0.5, 0.5, -0.5, 0.5 };
	DoubleBoundingBox3D bb_unit(bb_hard);

	vtkTransformPtr transform = vtkTransformPtr::New();
	mBoxWidget->GetTransform(transform);
	Transform3D M(transform->GetMatrix());

	Transform3D rMd = mImage->get_rMd();
	M = rMd.inv() * M;

	DoubleBoundingBox3D bb_new_r = cx::transform(M, bb_unit);

	return bb_new_r;
}

void InteractiveCropper::setCroppingRegion(DoubleBoundingBox3D bb_d)
{
	if (!mImage)
		return;
	mImage->setCroppingBox(bb_d);
	emit changed();
}

void InteractiveCropper::boxWasShown(bool val)
{
	emit changed();
}

/** return the largest useful bounding box for the current selection
 *
 */
DoubleBoundingBox3D InteractiveCropper::getMaxBoundingBox()
{
	if (!mImage)
		return DoubleBoundingBox3D::zero();
	return mImage->boundingBox();
}

} // namespace cx
