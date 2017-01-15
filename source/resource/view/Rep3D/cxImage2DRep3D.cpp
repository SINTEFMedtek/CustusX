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
#include "cxImage2DRep3D.h"

#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "cxLogger.h"

#include "cxSlicedImageProxy.h"
#include "cxImage.h"
#include "cxView.h"
#include "cxImageLUT2D.h"

//---------------------------------------------------------
namespace cx
{
//---------------------------------------------------------

Image2DProxy::Image2DProxy()
{
	m_rMrr = Transform3D::Identity();
	mActor = vtkImageActorPtr::New();
	mImageWithLUTProxy.reset(new ApplyLUTToImage2DProxy());
}

Image2DProxy::~Image2DProxy()
{
	mImage.reset();
}

Image2DProxyPtr Image2DProxy::New()
{
	return Image2DProxyPtr(new Image2DProxy());
}

vtkImageActorPtr Image2DProxy::getActor()
{
	return mActor;
}

void Image2DProxy::setImage(ImagePtr image)
{

	if (image==mImage)
	{
		return;
	}

	if (mImage)
	{
		disconnect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		disconnect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
	}

	mImage = image;

	if (mImage)
	{
		connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
		connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		connect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
	}

	if (mImage)
	{
		CX_ASSERT(mImage->is2D()); // class only treats 2d images.

		mImageWithLUTProxy->setInputData(mImage->getBaseVtkImageData(), mImage->getLookupTable2D()->getOutputLookupTable());
	}
	else
	{
		mImageWithLUTProxy->setInputData(vtkImageDataPtr(), vtkLookupTablePtr());
	}

	this->vtkImageDataChangedSlot();
	this->transformChangedSlot();
}

void Image2DProxy::vtkImageDataChangedSlot()
{
	mActor->SetInputData(mImageWithLUTProxy->getOutput());
}

/**called when transform is changed
 * reset it in the prop.*/
void Image2DProxy::transformChangedSlot()
{
	if (!mImage)
	{
		return;
	}

	Transform3D rrMd = mImage->get_rMd();
	Transform3D rMd = m_rMrr * rrMd;

	mActor->SetUserMatrix(rMd.getVtkMatrix());
}

void Image2DProxy::transferFunctionsChangedSlot()
{
	mImageWithLUTProxy->setInputData(mImage->getBaseVtkImageData(), mImage->getLookupTable2D()->getOutputLookupTable());
}

void Image2DProxy::setTransformOffset(Transform3D rMrr)
{
	m_rMrr = rMrr;
	this->transformChangedSlot();
}



// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


Image2DRep3D::Image2DRep3D() :
	RepImpl("")
{
	mProxy = Image2DProxy::New();
}

Image2DRep3D::~Image2DRep3D()
{
}

void Image2DRep3D::setImage(ImagePtr image)
{
	mProxy->setImage(image);
}

void Image2DRep3D::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddActor(mProxy->getActor());
}

void Image2DRep3D::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(mProxy->getActor());
}


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

