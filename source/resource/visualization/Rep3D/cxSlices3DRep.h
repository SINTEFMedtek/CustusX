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


/*
 * sscSlices3DRep.h
 *
 *  Created on: Oct 13, 2011
 *      Author: christiana
 */

#ifndef CXSLICES3DREP_H_
#define CXSLICES3DREP_H_

#include "cxRepImpl.h"
#include <vector>
#include "cxTransform3D.h"
#include "cxBoundingBox3D.h"
#include "cxDefinitions.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

//---------------------------------------------------------
namespace cx
{
class DataManager;
typedef boost::shared_ptr<class Slices3DRep> Slices3DRepPtr;
typedef boost::shared_ptr<class Texture3DSlicerProxy> Texture3DSlicerProxyPtr;

/** \brief Display several slices through volumes in 3D.
 *
 * Use this as an alternative to full volume rendering.
 * Define a set of planes and an ordered set of images.
 * The images are sliced in a way similar to that in
 * Texture3DSlicerRep, but they are displayed in the
 * 3D View instead of in 2D.
 *
 * Used by CustusX.
 *
 * \sa Texture3DSlicerProxy
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
 */
class Slices3DRep: public RepImpl
{
Q_OBJECT
public:
	static Slices3DRepPtr New(const QString& uid);
	virtual ~Slices3DRep();
	virtual QString getType() const { return "Slices3DRep"; }

	void setShaderPath(QString path);
	void setImages(std::vector<ImagePtr> images);
	void addPlane(PLANE_TYPE plane, DataServicePtr dataManager);
	void setTool(ToolPtr tool);

protected:
	Slices3DRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

private:
	View *mView;
	std::vector<Texture3DSlicerProxyPtr> mProxy;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

#endif /* CXSLICES3DREP_H_ */
