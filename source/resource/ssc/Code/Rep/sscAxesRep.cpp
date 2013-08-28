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

#include "sscAxesRep.h"

#include "boost/bind.hpp"
#include <vtkAxesActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkAssembly.h>

#include "sscTypeConversions.h"
#include "sscView.h"
#include "sscGraphicalPrimitives.h"
#include "cxGraphicalAxes3D.h"

namespace ssc
{

AxesRepPtr AxesRep::New(const QString& uid)
{
    AxesRepPtr retval(new AxesRep(uid));
    retval->mSelf = retval;
    return retval;
}

AxesRep::AxesRep(const QString& uid) :
	RepImpl(uid)
{
    mAxes.reset(new GraphicalAxes3D());
	this->setAxisLength(0.2);
	this->setShowAxesLabels(true);
    this->setFontSize(0.04);
}

AxesRep::~AxesRep()
{
}

void AxesRep::addRepActorsToViewRenderer(View *view)
{
    mAxes->setRenderer(view->getRenderer());
}

void AxesRep::removeRepActorsFromViewRenderer(View *view)
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


} // namespace ssc
