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


#ifndef CXGEOMETRICREP2D_H_
#define CXGEOMETRICREP2D_H_

#include "cxResourceVisualizationExport.h"

#include "vtkForwardDeclarations.h"
#include "cxRepImpl.h"
#include "cxVector3D.h"

namespace cx
{
typedef boost::shared_ptr<class Mesh> MeshPtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;

typedef boost::shared_ptr<class GeometricRep2D> GeometricRep2DPtr;

/**\brief Display one Mesh in 2D.
 *
 * Use this to render geometric polydata in a 2D scene
 * as an intersection between the full polydata and the slice plane.
 *
 * Used by CustusX.
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 */
class cxResourceVisualization_EXPORT GeometricRep2D : public RepImpl
{
	Q_OBJECT
public:
	virtual ~GeometricRep2D();

	static GeometricRep2DPtr New(const QString& uid, const QString& name="");

	virtual QString getType() const { return "GeometricRep2D"; } ///< gives this reps type
	void setMesh(MeshPtr mesh); ///< sets this reps mesh
	MeshPtr getMesh(); ///< gives this reps mesh
	bool hasMesh(MeshPtr mesh) const; ///< checks if this rep has the give mesh
	void setSliceProxy(SliceProxyPtr slicer);

protected:
	GeometricRep2D(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

	vtkPolyDataMapperPtr mMapper;
	vtkPropertyPtr mProperty;
	vtkActorPtr mActor;

	MeshPtr mMesh;
	SliceProxyPtr mSlicer;

private slots:
	void meshChangedSlot();
	void transformChangedSlot();
};

} // namespace cx

#endif /*CXGEOMETRICREP2D_H_*/
