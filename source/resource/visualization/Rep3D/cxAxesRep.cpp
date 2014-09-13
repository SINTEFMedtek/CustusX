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


#include "cxAxesRep.h"

#include "boost/bind.hpp"
#include <vtkAxesActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkAssembly.h>

#include "cxTypeConversions.h"
#include "cxView.h"
#include "cxGraphicalPrimitives.h"
#include "cxGraphicalAxes3D.h"

namespace cx
{

AxesRepPtr AxesRep::New(const QString& uid)
{
	return wrap_new(new AxesRep(), uid);
}

AxesRep::AxesRep() :
	RepImpl()
{
    mAxes.reset(new GraphicalAxes3D());
	this->setAxisLength(0.2);
	this->setShowAxesLabels(true);
    this->setFontSize(0.04);
}

AxesRep::~AxesRep()
{
}

void AxesRep::addRepActorsToViewRenderer(ViewPtr view)
{
    mAxes->setRenderer(view->getRenderer());
}

void AxesRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
    mAxes->setRenderer(NULL);
}

void AxesRep::setVisible(bool on)
{
    mAxes->setVisible(on);
}

void AxesRep::setShowAxesLabels(bool on)
{
    mAxes->setShowAxesLabels(on);
}

void AxesRep::setCaption(const QString& caption, const Vector3D& color)
{
    mAxes->setCaption(caption, color);
}

void AxesRep::setFontSize(double size)
{
    mAxes->setFontSize(size);
}

void AxesRep::setAxisLength(double length)
{
    mAxes->setAxisLength(length);
}

void AxesRep::setTransform(Transform3D rMt)
{
    mAxes->setTransform(rMt);
}


} // namespace cx
