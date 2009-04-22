#include "sscSlicerRepSW.h"

#include <boost/lexical_cast.hpp>
#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageBlend.h>
#include <vtkLookupTable.h>

#include <vtkColorTransferFunction.h>
#include "sscView.h"
#include "sscDataManager.h"
#include "sscSliceProxy.h"

#include "sscBoundingBox3D.h"

//#define USE_TRANSFORM_RESCLICER

namespace ssc
{

SliceRepSW::SliceRepSW(const std::string& uid) :
	RepImpl(uid)
{
//	std::cout << "SliceRepSW created: " << uid << std::endl;
	mReslicer = vtkImageReslicePtr::New();
	mMatrixAxes = vtkMatrix4x4Ptr::New();
	mImageActor = vtkImageActorPtr::New();

	//mWindowLevel = vtkImageMapToWindowLevelColorsPtr::New();
	mWindowLevel = vtkImageMapToColorsPtr::New();

	// set up the slicer pipeline
	mReslicer->SetInterpolationModeToLinear();
	mReslicer->SetOutputDimensionality(2);
	mReslicer->SetResliceAxes(mMatrixAxes) ;
	mReslicer->SetAutoCropOutput(false);
	
	

}
SliceRepSW::~SliceRepSW()
{

}
SliceRepSWPtr SliceRepSW::New(const std::string& uid)
{
	SliceRepSWPtr retval(new SliceRepSW(uid));
	retval->mSelf = retval;
	return retval;
}

/**This method brings vtkImageData that is preprocessed
 *with color
 */
void SliceRepSW::setInput(vtkImageDataPtr input)
{
	std::cout << "DANGEROUS METHOD CALLED: " << getName() << std::endl;

	if(!input)
		 return;

	if (mImage)
	{
		mImage->connectRep(mSelf);
		mImage.reset();
	}
	mReslicer->SetInput(input);
	mImageActor->SetInput( mReslicer->GetOutput() );
}
/**This method set the image, that has all the information in it self.
 * color, brigthness, contrast, etc...
 */
void SliceRepSW::setImage( ImagePtr image )
{
	if (mImage)
	{
		mImage->disconnectRep(mSelf);
	}
	mImage = image;
	if (mImage)
	{
		mImage->connectRep(mSelf);
#ifdef USE_TRANSFORM_RESCLICER
		mReslicer->SetInput(mImage->getRefVtkImageData());
#else
		mReslicer->SetInput(mImage->getBaseVtkImageData());
#endif
		mWindowLevel->SetInputConnection( mReslicer->GetOutputPort() );
		mWindowLevel->SetOutputFormatToRGBA();
		mWindowLevel->SetLookupTable(image->getLookupTable2D().getLookupTable());
		mWindowLevel->Update();
	}
}

std::string SliceRepSW::getImageUid()const
{
	return mImage ? mImage->getUid() : "";
}

void SliceRepSW::setSliceProxy(ssc::SliceProxyPtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
	}
	mSlicer = slicer;
	connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
}

void SliceRepSW::addRepActorsToViewRenderer(View* view)
{
	mImageActor->SetInput( mWindowLevel->GetOutput());
	view->getRenderer()->AddActor(mImageActor);
}
void SliceRepSW::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mImageActor);
}
bool SliceRepSW::hasImage(ImagePtr image) const
{
	return (mImage != NULL);
}

void SliceRepSW::sliceTransformChangedSlot(Transform3D sMr)
{
	update();
}		

void SliceRepSW::update()
{
	Transform3D rMs = mSlicer->get_sMr().inv();
	Transform3D iMr = mImage->get_rMd().inv();	
	Transform3D M = iMr*rMs;
	
#ifdef USE_TRANSFORM_RESCLICER
	mMatrixAxes->DeepCopy(rMs.matrix());
#else
	mMatrixAxes->DeepCopy(M.matrix());
#endif
}


void SliceRepSW::printSelf(std::ostream & os, Indent indent)
{
	RepImpl::printSelf(os, indent);

	//os << indent << "PlaneType: " << mType << std::endl;
	os << indent << "mImage: " << (mImage ? mImage->getUid() : "NULL") << std::endl;
	os << indent << "mSlicer: " << (mSlicer ? mSlicer.get() : 0) << std::endl;
	if (mSlicer)
	{
		mSlicer->printSelf(os, indent.stepDown());
	}
	os << indent << "mReslicer->GetOutput(): " << mReslicer->GetOutput() << std::endl;
	os << indent << "mReslicer->GetInput() : " << mReslicer->GetInput() << std::endl;
	Transform3D test(mReslicer->GetResliceAxes());
	os << indent << "resliceaxes: " << std::endl;
	test.put(os, indent.getIndent()+3);
	os << std::endl;
	//os << indent << "rMs_debug: " << std::endl;
	//rMs_debug.put(os, indent.getIndent()+3);

}

}// namespace ssc
