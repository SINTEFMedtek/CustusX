/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
