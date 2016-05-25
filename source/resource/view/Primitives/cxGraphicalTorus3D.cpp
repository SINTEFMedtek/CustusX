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
	source->SetToroidal(true);
	source->SetSize(10);
	source->SetThickness(0.5);
	source->SetThetaResolution(source->GetThetaResolution()*2);
}

GraphicalTorus3D::~GraphicalTorus3D()
{
}

void GraphicalTorus3D::setRadius(double value)
{
	source->SetSize(value);
}

void GraphicalTorus3D::setThickness(double value)
{
	source->SetThickness(value);
}

void GraphicalTorus3D::setColor(QColor color)
{
	setColorAndOpacity(actor->GetProperty(), color);
}

} // namespace cx

