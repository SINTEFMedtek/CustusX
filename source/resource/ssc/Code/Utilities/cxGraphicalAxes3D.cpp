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

#include "cxGraphicalAxes3D.h"

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

namespace ssc
{

GraphicalAxes3D::GraphicalAxes3D(vtkRendererPtr renderer)
{
    mViewportListener.reset(new ssc::ViewportListener);
    mViewportListener->setCallback(boost::bind(&GraphicalAxes3D::rescale, this));

    mAssembly = vtkAssemblyPtr::New();
    mActor = vtkAxesActorPtr::New();
    mAssembly->AddPart(mActor);
    this->setAxisLength(0.2);

    this->setShowAxesLabels(true);
    setTransform(Transform3D::Identity());
    setFontSize(0.04);

    this->setRenderer(renderer);
}

void GraphicalAxes3D::setRenderer(vtkRendererPtr renderer)
{
    if (mRenderer)
    {
        mRenderer->RemoveActor(mAssembly);
        for (unsigned i=0; i<mCaption.size(); ++i)
            mRenderer->RemoveActor(mCaption[i]);
        mViewportListener->stopListen();
    }

    mRenderer = renderer;

    if (mRenderer)
    {
        mRenderer->AddActor(mAssembly);
        for (unsigned i=0; i<mCaption.size(); ++i)
            mRenderer->AddActor(mCaption[i]);
        mViewportListener->startListen(mRenderer);
        this->rescale();
    }
}

GraphicalAxes3D::~GraphicalAxes3D()
{
    this->setRenderer(NULL);
}

void GraphicalAxes3D::rescale()
{
    if (!mViewportListener->isListening())
        return;
    double size = mViewportListener->getVpnZoom();
    double axisSize = mSize/size;

    mActor->SetTotalLength( axisSize, axisSize, axisSize );
    setTransform(Transform3D(mAssembly->GetUserMatrix()));
}

void GraphicalAxes3D::setVisible(bool on)
{
    mAssembly->SetVisibility(on);
    for (unsigned i=0; i<mCaption.size(); ++i)
        mCaption[i]->SetVisibility(on);
}

void GraphicalAxes3D::setShowAxesLabels(bool on)
{
    if (on)
    {
        this->addCaption("x", Vector3D(1,0,0), Vector3D(1,0,0));
        this->addCaption("y", Vector3D(0,1,0), Vector3D(0,1,0));
        this->addCaption("z", Vector3D(0,0,1), Vector3D(0,0,1));
    }
    else
    {
        if (mRenderer)
        {
            for (unsigned i=0; i<mCaption.size(); ++i)
                mRenderer->RemoveActor(mCaption[i]);
        }
        mCaption.clear();
        mCaptionPos.clear();
    }
}

void GraphicalAxes3D::setCaption(const QString& caption, const Vector3D& color)
{
    this->addCaption(caption, Vector3D(0,0,0), color);
}

/**set font size to a fraction of the normalized viewport.
 *
 */
void GraphicalAxes3D::setFontSize(double size)
{
    mFontSize = size;

    for (unsigned i=0; i<mCaption.size(); ++i)
    {
        mCaption[i]->SetHeight(mFontSize);
    }
}

/**set axis length to a world length
 *
 */
void GraphicalAxes3D::setAxisLength(double length)
{
    mSize = length;
    this->rescale();
}

/**Set the position of the axis.
 *
 */
void GraphicalAxes3D::setTransform(Transform3D rMt)
{
    mAssembly->SetUserMatrix(rMt.getVtkMatrix());

    for (unsigned i=0; i<mCaption.size(); ++i)
    {
        if (!mViewportListener->isListening())
            continue;
        double size = mViewportListener->getVpnZoom();
        double axisSize = mSize/size;

        Vector3D pos = rMt.coord(axisSize*mCaptionPos[i]);
        mCaption[i]->SetAttachmentPoint(pos.begin());
    }
}

void GraphicalAxes3D::addCaption(const QString& label, Vector3D pos, Vector3D color)
{
    vtkCaptionActor2DPtr cap = vtkCaptionActor2DPtr::New();
    cap->SetCaption(cstring_cast(label));
    cap->GetCaptionTextProperty()->SetColor(color.begin());
    cap->LeaderOff();
    cap->BorderOff();
    cap->GetCaptionTextProperty()->ShadowOff();
    mCaption.push_back(cap);
    mCaptionPos.push_back(pos);
}


} // namespace ssc
