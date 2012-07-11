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

#ifndef SSCGeometricRep2D_H_
#define SSCGeometricRep2D_H_

#include "vtkForwardDeclarations.h"
#include "sscRepImpl.h"
#include "sscVector3D.h"

namespace ssc
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
 * \ingroup sscRep
 * \ingroup sscRep2D
 */
class GeometricRep2D : public RepImpl
{
	Q_OBJECT
public:
	virtual ~GeometricRep2D();

	static GeometricRep2DPtr New(const QString& uid, const QString& name="");

	virtual QString getType() const { return "ssc::GeometricRep2D"; } ///< gives this reps type
	void setMesh(MeshPtr mesh); ///< sets this reps mesh
	MeshPtr getMesh(); ///< gives this reps mesh
	bool hasMesh(MeshPtr mesh) const; ///< checks if this rep has the give mesh
	void setSliceProxy(ssc::SliceProxyPtr slicer);

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

} // namespace ssc

#endif /*SSCGeometricRep2D_H_*/
