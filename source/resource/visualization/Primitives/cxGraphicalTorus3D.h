// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXGRAPHICALTORUS3D_H
#define CXGRAPHICALTORUS3D_H

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"
class QColor;

typedef vtkSmartPointer<class vtkSuperquadricSource> vtkSuperquadricSourcePtr;

namespace cx
{

/** \brief Helper for rendering a torus in 3D
 *
 * \ingroup cx
 * \date 12.02.2014-02-12
 * \author christiana
 */
class GraphicalTorus3D
{
	public:
		GraphicalTorus3D(vtkRendererPtr renderer = vtkRendererPtr());
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
		~GraphicalTorus3D();
		void setRadius(double value);
		void setThickness(double radius);
		void setColor(QColor color);
		void setPosition(Vector3D point);
		void setDirection(Vector3D direction);
		vtkActorPtr getActor();
		vtkPolyDataPtr getPolyData();

	private:
		vtkSuperquadricSourcePtr source;
		vtkPolyDataMapperPtr mapper;
		vtkActorPtr actor;
		vtkRendererPtr mRenderer;
};
typedef boost::shared_ptr<GraphicalTorus3D> GraphicalTorus3DPtr;


} // namespace cx

#endif // CXGRAPHICALTORUS3D_H
