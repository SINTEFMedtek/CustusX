/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSlider2D.h"
#include "cxActiveData.h"
#include "cxImage.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include "cxSliceProxy.h"
#include "cxTrackingService.h"
#include "cxVisServices.h"
#include "vtkImageData.h"

namespace cx
{
Slider2D::Slider2D(VisServicesPtr services, SliceProxyPtr sliceProxy, ctkDoubleSlider *slider) :
	mServices(services),
	mSlider(slider),
	mLastSliderValue(0),
	mSliceProxy(sliceProxy)
{
	this->imageChanged();
	connect(mSlider, &ctkDoubleSlider::valueChanged, this, &Slider2D::sliderChanged);

	mActiveImageProxy = ActiveImageProxy::New(mServices->patient()->getActiveData());
	connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &Slider2D::imageChanged);
	mActiveTool = ActiveToolProxy::New(mServices->tracking());
	connect(mActiveTool.get(), &ActiveToolProxy::toolTransformAndTimestamp, this, &Slider2D::updateSliderPosition);
}

void Slider2D::imageChanged()
{
	if(!mSlider)
		return;
	ImagePtr image = this->getImage();
	if(!image)
		return;

	int dim = this->getDimension();
	double dimsize = image->getBaseVtkImageData()->GetDimensions()[dim];
	mSlider->blockSignals(true);
	mSlider->setRange(0, dimsize);
	mSlider->blockSignals(false);
	this->updateSliderPosition();
	mLastSliderValue = mSlider->sliderPosition();
}

ImagePtr Slider2D::getImage()
{
	ImagePtr image = mServices->patient()->getActiveData()->getActive<Image>();
	return image;
}

void Slider2D::updateSliderPosition()
{
	if(!mSlider)
		return;

	double outOfPlane_voxels = this->getOutOfPlaneVoxels();
	correctSliderValue(outOfPlane_voxels);

	mSlider->blockSignals(true);
	mLastSliderValue = mSlider->sliderPosition();
	mSlider->setSliderPosition(outOfPlane_voxels);
	mSlider->blockSignals(false);
}

double Slider2D::getOutOfPlaneVoxels()
{
	ImagePtr image = this->getImage();
	if(!image)
		return 0;
	int dim = this->getDimension();
	Vector3D spacing = image->getSpacing();
	Vector3D tool_d = this->get_tool_d();
	double outOfPlane_voxels = tool_d[dim] / spacing[dim];
	return outOfPlane_voxels;
}

int Slider2D::getDimension()
{
	int dim = 0;
	PLANE_TYPE plane = this->getPlaneType();
	if(plane == ptAXIAL)
		dim = 2;//z dim for axial
	if(plane == ptCORONAL)
		dim = 1;//y dim
	if(plane == ptSAGITTAL)
		dim = 0;//x dim
	return dim;
}

PLANE_TYPE Slider2D::getPlaneType()
{
	PLANE_TYPE plane = mSliceProxy->getComputer().getPlaneType();
	return plane;
}

Vector3D Slider2D::get_tool_d()
{
	ToolPtr tool = mServices->tracking()->getManualTool();
	ImagePtr image = this->getImage();
	if(!tool || !image)
		return Vector3D::Identity();

	Transform3D rMpr = mServices->patient()->get_rMpr();
	Transform3D prMt = tool->get_prMt();
	Transform3D rMd = image->get_rMd();

	Vector3D tool_t(0, 0, tool->getTooltipOffset());
	Vector3D tool_d = (rMd.inverse() * rMpr * prMt).coord(tool_t);
	return tool_d;
}

bool Slider2D::correctSliderValue(double &sliderValue)
{
	ImagePtr image = this->getImage();
	if(!image)
		return false;

	int dim = this->getDimension();
	double dimsize = image->getBaseVtkImageData()->GetDimensions()[dim];
	bool retval = true;
	if(sliderValue > dimsize)
		sliderValue = dimsize;
	else if(sliderValue < 0)
		sliderValue = 0;
	else
		retval = false;
	return retval;
}

void Slider2D::sliderChanged(double sliderValue)
{
	double sliderValueDiff = sliderValue - mLastSliderValue;
	mLastSliderValue = sliderValue;
	this->updateSliderDiffIfOutOfRange(sliderValueDiff);
	Vector3D delta_d;

	PLANE_TYPE plane = this->getPlaneType();
	if(plane == ptAXIAL)
		delta_d = Vector3D(0, 0, sliderValueDiff);
	else if(plane == ptCORONAL)
		delta_d = Vector3D(0, sliderValueDiff, 0);
	else if(plane == ptSAGITTAL)
		delta_d = Vector3D(sliderValueDiff, 0, 0);

	this->shiftPosOutOfPlane(delta_d);
}

void Slider2D::updateSliderDiffIfOutOfRange(double &sliderValueDiff)
{
	double outOfPlane_voxels = this->getOutOfPlaneVoxels();
	double newDiff = mLastSliderValue - outOfPlane_voxels;
	bool correctedSliderValue = correctSliderValue(outOfPlane_voxels);
	if(correctedSliderValue)
	{
		sliderValueDiff = newDiff;
		mLastSliderValue = outOfPlane_voxels;
	}
}

void Slider2D::shiftPosOutOfPlane(Vector3D delta_d_voxels)
{
	ToolPtr tool = mServices->tracking()->getManualTool();
	if(!tool)
		return;
	Transform3D sMr = this->get_sMr();
	Transform3D rMpr = mServices->patient()->get_rMpr();
	Transform3D prMt = tool->get_prMt();

	ImagePtr image = this->getImage();
	Vector3D spacing = image->getSpacing();
	Vector3D delta_d_mm = Vector3D(delta_d_voxels[0]*spacing[0], delta_d_voxels[1]*spacing[1], delta_d_voxels[2]*spacing[2]);
	Transform3D MD = createTransformTranslate(delta_d_mm);

	Transform3D rMd = image->get_rMd();
	Transform3D dMpr = rMd.inv() * rMpr;

	tool->set_prMt(dMpr.inv() * MD * dMpr * prMt);
}

Transform3D Slider2D::get_sMr()
{
	return mSliceProxy->get_sMr();
}

}//cx
