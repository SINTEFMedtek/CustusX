#include "sscSlicedImageProxy.h"

#include <boost/lexical_cast.hpp>
#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

#include "sscView.h"
#include "sscDataManager.h"
#include "sscSliceProxy.h"

namespace ssc
{

SlicedImageProxy::SlicedImageProxy()
{
	mMatrixAxes = vtkMatrix4x4Ptr::New();
	mWindowLevel = vtkImageMapToWindowLevelColorsPtr::New();
	mReslicer = vtkImageReslicePtr::New();
	mReslicer->SetInterpolationModeToLinear();
	mReslicer->SetOutputDimensionality( 2);
	mReslicer->SetResliceAxes(mMatrixAxes);
	mReslicer->SetAutoCropOutput(false);
	mWindowLevel->SetInputConnection( mReslicer->GetOutputPort() );
}

SlicedImageProxy::~SlicedImageProxy()
{
}

void SlicedImageProxy::setSliceProxy(SliceProxyPtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
	}
	mSlicer = slicer;
	connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));

}

void SlicedImageProxy::setImage(ImagePtr image)
{
	std::cout<<"got image id"<<image->getUid()<<std::endl;

//	if (mImage)
//	{
//		disconnect( mImage.get(), SIGNAL(alphaChange()), this, SIGNAL(updateAlpha()));
//	}

//	connect( mImage.get(), SIGNAL(alphaChange()), this, SIGNAL(updateAlpha()));

	mImage = image;
	//mReslicer->SetInput(mImage->getRefVtkImageData());	
	mReslicer->SetInput(mImage->getBaseVtkImageData());
	mWindowLevel->SetLookupTable( image->getLookupTable2D().getLookupTable() );
	mWindowLevel->SetOutputFormatToRGBA();
	mWindowLevel->Update();
}

ImagePtr SlicedImageProxy::getImage()const
{
	return mImage;
}

//deliver the sliced image..
vtkImageDataPtr SlicedImageProxy::getOutput()
{
	return	mWindowLevel->GetOutput();
}

void SlicedImageProxy::update()
{
	Transform3D rMs = mSlicer->get_sMr().inv();
	Transform3D iMr = mImage->get_rMd().inv();
	Transform3D M = iMr*rMs;
	
	mMatrixAxes->DeepCopy(M.matrix());
	
//	Transform3D rMs = mSlicer->get_sMr().inv();
//	mMatrixAxes->DeepCopy(rMs.matrix());
}
void SlicedImageProxy::sliceTransformChangedSlot(Transform3D sMr)
{
	update();
}
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------




