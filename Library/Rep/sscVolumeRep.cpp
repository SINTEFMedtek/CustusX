#include "sscVolumeRep.h"

#include <vtkVolumeTextureMapper3D.h>
#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include "sscView.h"

namespace ssc
{

VolumeRep::VolumeRep(const std::string& uid, const std::string& name, ImagePtr image) : 
	RepImpl(uid, name)
{
	//mProxy->setImage(image);

	mTextureMapper3D = vtkVolumeTextureMapper3DPtr::New();
	mVolume = vtkVolumePtr::New();

	mTextureMapper3D->SetInput ( image->getVtkImageData() );	
	mVolume->SetMapper ( mTextureMapper3D );
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
