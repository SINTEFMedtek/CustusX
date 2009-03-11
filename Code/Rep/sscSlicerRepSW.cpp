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

namespace ssc
{

SliceRepSW::SliceRepSW(const std::string& uid) :
	RepImpl(uid)
{	
	mReslicer = vtkImageReslicePtr::New();
	mMatrixAxes = vtkMatrix4x4Ptr::New();
	mImageActor = vtkImageActorPtr::New();
	
	mWindowLevel = vtkImageMapToWindowLevelColorsPtr::New();
	mLookupTable = vtkWindowLevelLookupTablePtr::New();
	
	// set up the slicer pipeline
	mReslicer->SetInterpolationModeToLinear();
	mReslicer->SetOutputDimensionality(2);
	mReslicer->SetResliceAxes(mMatrixAxes) ;
	mReslicer->SetAutoCropOutput(false);
	
	mWindowLevel->SetInputConnection( mReslicer->GetOutputPort() );

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
	}
	mReslicer->SetInput(mImage->getRefVtkImageData());
	
	double from;
	double to; 
	vtkLookupTable *table =vtkLookupTable::SafeDownCast( image->lookupTable2D().getLookupTable());
	table->GetAlphaRange(from, to );
	std::cout<<"opacity from " <<from<<", to: "<<to <<std::endl;
	
	mWindowLevel->SetLookupTable(table);
	mWindowLevel->SetOutputFormatToRGBA();
	mWindowLevel->Update();	
	
	std::cout<<"Number of components "<< mWindowLevel->GetOutput()->GetNumberOfScalarComponents()<<std::endl;
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
	//std::cout << "slicerep get transform "+getName()+" :\n"+boost::lexical_cast<std::string>(rMs) << std::endl;
	mMatrixAxes->DeepCopy(rMs.matrix());
}

void SliceRepSW::setLookupTable(vtkScalarsToColorsPtr lut)	
{
	vtkWindowLevelLookupTablePtr newlut = vtkWindowLevelLookupTable::SafeDownCast(lut);
	if(!newlut)	
	{
		std::cout<<"SliceRepSW, Cannot cast vtkScalarsToColors to  vtkImageMapToWindowLevelColors "<<std::endl;
	}
	mWindowLevel->SetLookupTable(newlut);
	mWindowLevel->SetWindow( 255.0);
	mWindowLevel->SetLevel( 127.5);
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
}

}// namespace ssc
