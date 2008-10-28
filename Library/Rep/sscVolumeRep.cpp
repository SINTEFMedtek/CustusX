#include "sscVolumeRep.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include "sscView.h"

namespace ssc
{

VolumeRep::VolumeRep(const std::string& uid, const std::string& name, ImagePtr image) : 
	RepImpl(uid, name),
	mOpacityTransferFunction(vtkPiecewiseFunctionPtr::New()),
	mColorTransferFunction(vtkColorTransferFunctionPtr::New()),
	mVolumeProperty(vtkVolumePropertyPtr::New())
{
	//mProxy->setImage(image);
	
	mOpacityTransferFunction->AddPoint(0.0, 0.0);
	mOpacityTransferFunction->AddPoint(255, 1.0);

	mColorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	mColorTransferFunction->AddRGBPoint(255.0, 1.0, 1.0, 1.0);

	mVolumeProperty->SetColor(mColorTransferFunction);
	mVolumeProperty->SetScalarOpacity(mOpacityTransferFunction);
	mVolumeProperty->SetInterpolationTypeToLinear();

	mTextureMapper3D = vtkVolumeTextureMapper3DPtr::New();
	mTextureMapper3D->SetInput( image->getVtkImageData() );

	mVolume = vtkVolumePtr::New();
	mVolume->SetProperty( mVolumeProperty );
	mVolume->SetMapper( mTextureMapper3D );
}

VolumeRep::~VolumeRep()
{ 
	// ??
}

void VolumeRep::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddVolume(mVolume);	
}

void VolumeRep::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveVolume(mVolume);		
}

} // namespace ssc
