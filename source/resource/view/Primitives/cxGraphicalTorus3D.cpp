/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxGraphicalTorus3D.h"

#include <QColor>
#include "cxVtkHelperClasses.h"
#include "vtkSuperquadricSource.h"

#include <vtkProperty.h>
#include <vtkActor.h>

namespace cx
{

GraphicalTorus3D::GraphicalTorus3D(vtkRendererPtr renderer)
    : GraphicalObjectWithDirection(renderer)
{
    mSource->SetToroidal(true);
    mSource->SetSize(10);
    mSource->SetThickness(0.5);
    mSource->SetThetaResolution(mSource->GetThetaResolution()*2);
}

GraphicalTorus3D::~GraphicalTorus3D()
{
}

void GraphicalTorus3D::setRadius(double value)
{
    mSource->SetSize(value);
}

void GraphicalTorus3D::setThickness(double value)
{
    mSource->SetThickness(value);
}

void GraphicalTorus3D::setColor(QColor color)
{
    setColorAndOpacity(mActor->GetProperty(), color);
}

} // namespace cx

