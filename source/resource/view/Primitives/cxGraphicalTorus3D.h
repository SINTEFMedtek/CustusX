/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXGRAPHICALTORUS3D_H
#define CXGRAPHICALTORUS3D_H

#include "cxResourceVisualizationExport.h"
#include "vtkForwardDeclarations.h"
#include "cxGraphicalObjectWithDirection.h"

class QColor;


namespace cx
{

/** \brief Helper for rendering a torus in 3D
 *
 * \ingroup cx_resource_view
 * \date 12.02.2014-02-12
 * \author christiana
 */
class cxResourceVisualization_EXPORT GraphicalTorus3D : public GraphicalObjectWithDirection
{
public:
    GraphicalTorus3D(vtkRendererPtr renderer = vtkRendererPtr());
    ~GraphicalTorus3D();
    void setRadius(double value);
    void setThickness(double radius);
    void setColor(QColor color);
};
typedef boost::shared_ptr<GraphicalTorus3D> GraphicalTorus3DPtr;


} // namespace cx

#endif // CXGRAPHICALTORUS3D_H
