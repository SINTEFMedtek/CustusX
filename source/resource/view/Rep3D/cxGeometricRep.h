/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXGEOMETRICREP_H_
#define CXGEOMETRICREP_H_


#include "vtkForwardDeclarations.h"
#include "cxRepImpl.h"
#include "cxVector3D.h"
#include "cxGraphicalPrimitives.h"


namespace cx
{
typedef boost::shared_ptr<class Mesh> MeshPtr;
typedef boost::shared_ptr<class GeometricRep> GeometricRepPtr;
typedef boost::shared_ptr<class GraphicalGeometric> GraphicalGeometricPtr;

/** \brief Display one Mesh in 3D.
 *
 * Use this to render geometric polydata (vtkPolyData) in a 3D scene.
 *
 * Used by CustusX.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT GraphicalGeometric : public QObject
{
	Q_OBJECT
public:
	GraphicalGeometric();
	virtual ~GraphicalGeometric();

	void setMesh(MeshPtr mesh); ///< sets this reps mesh
	MeshPtr getMesh(); ///< gives this reps mesh
	void setRenderer(vtkRendererPtr renderer);

	/**
	 * insert a transform between r and the mesh,
	 * such that M = rMrr*rMd
	 */
	void setTransformOffset(Transform3D rMrr);

protected:
	GraphicalPolyData3DPtr mGraphicalPolyDataPtr;
	GraphicalGlyph3DDataPtr mGraphicalGlyph3DDataPtr;
	MeshPtr mMesh;
	Transform3D m_rMrr;
	void clearClipping();
private:
	void meshChangedSlot();
	void transformChangedSlot();
	void clipPlanesChangedSlot();
};



/** \brief Display one Mesh in 3D.
 *
 * Use this to render geometric polydata (vtkPolyData) in a 3D scene.
 *
 * Used by CustusX.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT GeometricRep : public RepImpl
{
	Q_OBJECT
public:
	virtual ~GeometricRep();
	static GeometricRepPtr New(const QString& uid="");

	virtual QString getType() const { return "GeometricRep"; } ///< gives this reps type
	void setMesh(MeshPtr mesh); ///< sets this reps mesh
	MeshPtr getMesh(); ///< gives this reps mesh
	bool hasMesh(MeshPtr mesh) const; ///< checks if this rep has the give mesh

protected:
	GeometricRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

private:
	GraphicalGeometricPtr mGraphics;
};






} // namespace cx

#endif /*CXGEOMETRICREP_H_*/
