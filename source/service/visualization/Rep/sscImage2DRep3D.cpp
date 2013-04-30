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
#include "sscImage2DRep3D.h"

#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"

#include "sscLogger.h"
#include "sscSlicedImageProxy.h"
#include "sscImage.h"
#include "sscView.h"
#include "sscImageLUT2D.h"

//---------------------------------------------------------
namespace cx
{
//---------------------------------------------------------

Image2DProxy::Image2DProxy()
{
	mActor = vtkImageActorPtr::New();
	mImageWithLUTProxy.reset(new ssc::ApplyLUTToImage2DProxy());
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

void Image2DProxy::setImage(ssc::ImagePtr image)
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
		SSC_ASSERT(mImage->getBaseVtkImageData()->GetDimensions()[2]==1); // class only treats 2d images.

		mImageWithLUTProxy->setInput(mImage->getBaseVtkImageData(), mImage->getLookupTable2D()->getOutputLookupTable());
	}
	else
	{
		mImageWithLUTProxy->setInput(vtkImageDataPtr(), vtkLookupTablePtr());
	}

	this->vtkImageDataChangedSlot();
	this->transferFunctionsChangedSlot();
	this->transformChangedSlot();
}

void Image2DProxy::vtkImageDataChangedSlot()
{
	mActor->SetInput(mImageWithLUTProxy->getOutput());
}

/**called when transform is changed
 * reset it in the prop.*/
void Image2DProxy::transformChangedSlot()
{
	if (!mImage)
	{
		return;
	}
	mActor->SetUserMatrix(mImage->get_rMd().getVtkMatrix());
}

void Image2DProxy::transferFunctionsChangedSlot()
{
	mImageWithLUTProxy->setInput(mImage->getBaseVtkImageData(), mImage->getLookupTable2D()->getOutputLookupTable());
}




// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


Image2DRep3D::Image2DRep3D(const QString& uid) :
	RepImpl(uid)
{
	mProxy = Image2DProxy::New();
	mView = NULL;
}

Image2DRep3D::~Image2DRep3D()
{
}

Image2DRep3DPtr Image2DRep3D::New(const QString& uid)
{
	Image2DRep3DPtr retval(new Image2DRep3D(uid));
	retval->mSelf = retval;
	return retval;
}

void Image2DRep3D::setImage(ssc::ImagePtr image)
{
	mProxy->setImage(image);
}

void Image2DRep3D::addRepActorsToViewRenderer(ssc::View *view)
{
	view->getRenderer()->AddActor(mProxy->getActor());
	mView = view;
}

void Image2DRep3D::removeRepActorsFromViewRenderer(ssc::View *view)
{
	view->getRenderer()->RemoveActor(mProxy->getActor());
	mView = NULL;
}


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

