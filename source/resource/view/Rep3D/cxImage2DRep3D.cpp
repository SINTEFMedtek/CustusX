/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxImage2DRep3D.h"

#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
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
	mSlice = vtkImageSlicePtr::New();
	vtkNew<vtkImageSliceMapper> imageSliceMapper;
	mSlice->SetMapper(imageSliceMapper);

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

vtkImageSlicePtr Image2DProxy::getSlice()
{
	return mSlice;
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
	mSlice->GetMapper()->SetInputData(mImageWithLUTProxy->getOutput());
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

	mSlice->SetUserMatrix(rMd.getVtkMatrix());
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
	view->getRenderer()->AddViewProp(mProxy->getSlice());
}

void Image2DRep3D::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveViewProp(mProxy->getSlice());
}


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

