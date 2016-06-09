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

#include "cxGraphicalBox.h"

#include "cxView.h"

#include <vector>
#include <vtkTransform.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkVolumeMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkCommand.h>
#include <vtkBoxWidget2.h>
#include <vtkBoxWidget.h>
#include "cxTypeConversions.h"
#include "cxBoundingBox3D.h"
#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxVolumetricRep.h"

#include "cxActiveImageProxy.h"
#include "cxActiveData.h"
#include "cxLogger.h"

namespace cx
{

class GraphicalBoxCallback: public vtkCommand
{
public:
	GraphicalBoxCallback() : mCropper(NULL)
	{
	}
	static GraphicalBoxCallback* New()
	{
		return new GraphicalBoxCallback;
	}
	void SetCropper(GraphicalBox* cropper)
	{
		mCropper = cropper;
	}
	virtual void Execute(vtkObject* caller, unsigned long, void*)
	{
		mCropper->updateBoxFromWidget();
	}
	GraphicalBox* mCropper;
};

class GraphicalBoxEnableCallback: public vtkCommand
{
public:
	GraphicalBoxEnableCallback() : mCropper(NULL), mValue(false)
	{
	}
	static GraphicalBoxEnableCallback* New()
	{
		return new GraphicalBoxEnableCallback;
	}
	void SetCropper(bool val, GraphicalBox* cropper)
	{
		mValue = val;
		mCropper = cropper;
	}
	virtual void Execute(vtkObject* caller, unsigned long, void*)
	{
		mCropper->setVisible(mValue);
	}
	bool mValue;
	GraphicalBox* mCropper;
};

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


GraphicalBox::GraphicalBox()
{
	mBox = DoubleBoundingBox3D::zero();
	mMaxBox = DoubleBoundingBox3D::zero();
	mInteractive = true;
	mVisible = true;
	m_rMd = Transform3D::Identity();
}

GraphicalBox::~GraphicalBox()
{
	if (mBoxWidget)
	{
		mBoxWidget->SetInteractor(NULL);
	}
}

void GraphicalBox::setRenderWindow(vtkRenderWindowPtr renderWindow)
{
	mRenderWindow = renderWindow;
	this->updateBoxWidgetInteractor();
}

/** get current cropping box in ref coords
 */
DoubleBoundingBox3D GraphicalBox::getBoundingBox()
{
	return mBox;
}

void GraphicalBox::setBoundingBox(const DoubleBoundingBox3D& bb_d)
{
	if (similar(bb_d, mBox))
		return;

	mBox = bb_d;
	this->updateWidgetFromBox();
	emit changed();
}

void GraphicalBox::setPosition(const Transform3D &rMd)
{
	if (similar(rMd, m_rMd))
		return;

	m_rMd = rMd;
	this->updateWidgetFromBox();
	emit changed();
}

void GraphicalBox::setVisible(bool on)
{
	if (mVisible==on)
		return;
	mVisible = on;
	this->updateWidgetFromBox();
	emit changed();
}

bool GraphicalBox::getVisible() const
{
	return mVisible;
}

void GraphicalBox::setInteractive(bool on)
{
	if (mInteractive==on)
		return;
	mInteractive = on;
	this->updateWidgetFromBox();
	emit changed();
}

bool GraphicalBox::getInteractive() const
{
	return mInteractive;
}

/** return the largest useful bounding box for the current selection
 *
 */
DoubleBoundingBox3D GraphicalBox::getMaxBoundingBox()
{
	return mMaxBox;
}

void GraphicalBox::updateWidgetFromBox()
{
	this->updateBoxWidgetInteractor();
	this->setBoxWidgetSize(mBox, m_rMd);
}

void GraphicalBox::updateBoxFromWidget()
{
	DoubleBoundingBox3D bb_d = this->getCurrentBoxWidgetSize();
	mBox = bb_d;
	emit changed();
}

void GraphicalBox::initialize()
{
	if (mBoxWidget) // already initialized
		return;

	mBoxWidget = vtkBoxWidgetPtr::New();
	mBoxWidget->RotationEnabledOff();

	double bb_hard[6] =
	{ -1, 1, -1, 1, -1, 1 };
	mBoxWidget->PlaceWidget(bb_hard);

	mGraphicalBoxCallback = GraphicalBoxCallbackPtr::New();
	mGraphicalBoxCallback->SetCropper(this);
	mGraphicalBoxEnableCallback = GraphicalBoxEnableCallbackPtr::New();
	mGraphicalBoxEnableCallback->SetCropper(true, this);
	mGraphicalBoxDisableCallback = GraphicalBoxEnableCallbackPtr::New();
	mGraphicalBoxDisableCallback->SetCropper(false, this);

	mBoxWidget->SetInteractor(mRenderWindow->GetInteractor());

	mBoxWidget->SetEnabled(mVisible);
}

void GraphicalBox::updateBoxWidgetInteractor()
{
	if (!mRenderWindow)
		return;

	this->initialize();

	mBoxWidget->SetInteractor(mRenderWindow->GetInteractor());

	if (this->getInteractive())
	{
		mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mGraphicalBoxCallback);
		mBoxWidget->AddObserver(vtkCommand::EnableEvent, mGraphicalBoxEnableCallback);
		mBoxWidget->AddObserver(vtkCommand::DisableEvent, mGraphicalBoxDisableCallback);
	}

	if (!this->getInteractive())
	{
		mBoxWidget->RemoveObserver(vtkCommand::InteractionEvent);
		mBoxWidget->RemoveObserver(vtkCommand::EnableEvent);
		mBoxWidget->RemoveObserver(vtkCommand::DisableEvent);
	}

	mBoxWidget->SetScalingEnabled(mInteractive);
	mBoxWidget->SetTranslationEnabled(mInteractive);

	mBoxWidget->SetOutlineFaceWires(mInteractive);
	mBoxWidget->SetOutlineCursorWires(mInteractive);
	if (mInteractive)
		mBoxWidget->HandlesOn();
	else
		mBoxWidget->HandlesOff();

	mBoxWidget->SetEnabled(mVisible);
}

/** Set the box widget bounding box to the input box (given in data space)
 */
void GraphicalBox::setBoxWidgetSize(const DoubleBoundingBox3D& bb_d, Transform3D rMd)
{
	if (!mBoxWidget)
		return;

	double bb_hard[6] =	{ -0.5, 0.5, -0.5, 0.5, -0.5, 0.5 };
	DoubleBoundingBox3D bb_unit(bb_hard);
	Transform3D M = createTransformNormalize(bb_unit, bb_d);
	M = rMd * M;

	if (similar(M, this->getBoxTransform()))
		return;

	this->setBoxTransform(M);
}

Transform3D GraphicalBox::getBoxTransform()
{
	vtkTransformPtr transform = vtkTransformPtr::New();
	mBoxWidget->GetTransform(transform);
	Transform3D M(transform->GetMatrix());
	return M;
}
void GraphicalBox::setBoxTransform(const Transform3D& M)
{
	vtkTransformPtr transform = vtkTransformPtr::New();
	transform->SetMatrix(M.getVtkMatrix());
	mBoxWidget->SetTransform(transform);
}

/** return the box widget current size in data space
 */
DoubleBoundingBox3D GraphicalBox::getCurrentBoxWidgetSize()
{
	if (!mBoxWidget)
	{
		return DoubleBoundingBox3D::zero();
	}

	double bb_hard[6] =
	{ -0.5, 0.5, -0.5, 0.5, -0.5, 0.5 };
	DoubleBoundingBox3D bb_unit(bb_hard);

	Transform3D M = this->getBoxTransform();
	M = m_rMd.inv() * M;
	DoubleBoundingBox3D bb_new_d = cx::transform(M, bb_unit);

	return bb_new_d;
}



} // namespace cx
