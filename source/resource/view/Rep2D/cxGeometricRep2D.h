/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 */
class cxResourceVisualization_EXPORT GeometricRep2D : public RepImpl
{
	Q_OBJECT
public:
	virtual ~GeometricRep2D();

	static GeometricRep2DPtr New(const QString& uid="");

	virtual QString getType() const { return "GeometricRep2D"; } ///< gives this reps type
	void setMesh(MeshPtr mesh); ///< sets this reps mesh
	MeshPtr getMesh(); ///< gives this reps mesh
	bool hasMesh(MeshPtr mesh) const; ///< checks if this rep has the give mesh
	void setSliceProxy(SliceProxyPtr slicer);

protected:
	GeometricRep2D();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

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
