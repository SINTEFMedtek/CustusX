// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscSliceRep.h"

#include <vtkImagePlaneWidget.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include "sscView.h"
#include "sscImage.h"

namespace ssc
{
SliceRep::SliceRep(const QString& uid) :
	RepImpl(uid)
{
	mVtkImagePlaneWidget = vtkImagePlaneWidgetPtr::New();
	mVtkImagePlaneWidget->SetPlaneOrientationToZAxes();//Convenience method sets the plane orientation normal to the x, y, or z axes
}

SliceRep::~SliceRep()
{
}

SliceRepPtr SliceRep::New(const QString& uid)
{
	SliceRepPtr retval(new SliceRep(uid));
	retval->mSelf = retval;
	return retval;
}

void SliceRep::addRepActorsToViewRenderer(View *view)
{
	mVtkImagePlaneWidget->SetInteractor( view->getRenderWindow()->GetInteractor() );
	mVtkImagePlaneWidget->InteractionOn();
	mVtkImagePlaneWidget->On();
}

void SliceRep::removeRepActorsFromViewRenderer(View *view)
{
}
void SliceRep::setImage(ImagePtr image)
{
	mImage = image;
	mImage->connectToRep(mSelf);
	mVtkImagePlaneWidget->SetInput( image->getBaseVtkImageData() );
}

bool SliceRep::hasImage(ImagePtr image) const
{
	return (mImage != NULL);
}

} // namespace ssc
