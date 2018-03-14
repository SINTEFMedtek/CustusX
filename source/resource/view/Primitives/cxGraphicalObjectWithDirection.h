/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef GRAPHICALOBJECTWITHDIRECTION_H
#define GRAPHICALOBJECTWITHDIRECTION_H

#include "cxResourceVisualizationExport.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"

typedef vtkSmartPointer<class vtkSuperquadricSource> vtkSuperquadricSourcePtr;

namespace cx
{

/** \brief Base helper class for rendering objects with a specific direction in 3D
 *
 * \ingroup cx_resource_view
 * \date 25.05.2016-05-25
 * \author jone
 */
class cxResourceVisualization_EXPORT GraphicalObjectWithDirection
{
public:
    GraphicalObjectWithDirection(vtkRendererPtr renderer = vtkRendererPtr());
    virtual ~GraphicalObjectWithDirection();

    vtkActorPtr getActor() const;
    vtkPolyDataPtr getPolyData() const;
    vtkPolyDataMapperPtr getMapper() const;
    void setPosition(Vector3D point);
    void setDirection(Vector3D direction);
    void setVectorUp(const Vector3D &up);
	void setScale(Vector3D scale);
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());

protected:
    void updateOrientation();

    vtkSuperquadricSourcePtr mSource;
    vtkPolyDataMapperPtr mMapper;
    vtkActorPtr mActor;
    vtkRendererPtr mRenderer;

    Vector3D mPoint;
    Vector3D mDirection;
    Vector3D mVectorUp;
	Vector3D mScale;
};
typedef boost::shared_ptr<GraphicalObjectWithDirection> GraphicalObjectWithDirectionPtr;

} // namespace cx

#endif // GRAPHICALOBJECTWITHDIRECTION_H
