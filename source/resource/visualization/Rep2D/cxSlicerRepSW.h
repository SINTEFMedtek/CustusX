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


#ifndef CXSLICERREPSW_H_
#define CXSLICERREPSW_H_

#include "cxRepImpl.h"

#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class SliceProxyInterface> SliceProxyInterfacePtr;


/**\brief Display an image slice in 2D.
 *
 * The image is sliced using a SliceProxy. Only a single
 * volume can be used.
 *
 * Used by CustusX.
 *
 * \sa SlicedImageProxy SliceProxy
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 */
class SliceRepSW : public RepImpl
{
	Q_OBJECT
public:
	static SliceRepSWPtr New(const QString& uid);
	virtual ~SliceRepSW();
	virtual QString getType() const { return "SliceRepSW"; };
	bool hasImage(ImagePtr image) const;
	void setImage(ImagePtr image);
	ImagePtr getImage();
	vtkImageActorPtr getActor();
	void setSliceProxy(SliceProxyInterfacePtr slicer);
	QString getImageUid()const;
	void update();
	virtual void printSelf(std::ostream & os, Indent indent);

protected:
	SliceRepSW(const QString& uid);
	virtual void addRepActorsToViewRenderer(View* view) ;
	virtual void removeRepActorsFromViewRenderer(View *view) ;

private:
	SlicedImageProxyPtr mImageSlicer;
	vtkImageActorPtr mImageActor;
};

}// namespace cx

#endif /*CXSLICERREPSW_H_*/
