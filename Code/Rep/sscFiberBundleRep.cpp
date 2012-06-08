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

#include "sscFiberBundleRep.h"

#include "boost/bind.hpp"
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkTubeFilter.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>

#include "sscGraphicalPrimitives.h"
#include "sscView.h"
#include "sscTypeConversions.h"
#include "sscDataManager.h"
#include "sscFiberBundle.h"

namespace ssc
{
/** Constructor */
FiberBundleRep::FiberBundleRep(const QString& uid, const QString& name)
    : RepImpl(uid, name), mFiberRadius(.05), mTubeFilter(true), mTubeSegments(8)
{
    mPolyDataMapper = vtkPolyDataMapperPtr::New();
    mProperty = vtkPropertyPtr::New();
    // mProperty->SetInterpolationToFlat();
    mProperty->SetInterpolationToGouraud();

    mActor = vtkActorPtr::New();

    mActor->SetMapper(mPolyDataMapper);
    mActor->SetProperty(mProperty);
}

/**
  * Creates a new smart pointer object of this rep
  */
FiberBundleRepPtr FiberBundleRep::New(const QString& uid, const QString& name)
{
    FiberBundleRepPtr retval(new FiberBundleRep(uid, name));
    retval->mSelf = retval;
    return retval;
}

/** Return type as string */
QString FiberBundleRep::getType() const
{
    return "ssc::FiberBundleRep";
}

/**
  * Assign new fiber bundle object
  */
void FiberBundleRep::setBundle(FiberBundlePtr bundle)
{
    // Already has this bundle assigned
    if (hasBundle(bundle)) return;

    std::cout << "New fiber bundle assigned" << std::endl;

    if (mBundle)
    {
        disconnect(mBundle.get(), SIGNAL(transformChanged()), this, SLOT(bundleTransformChanged()));
        disconnect(mBundle.get(), SIGNAL(bundleChanged()), this, SLOT(bundleChanged()));
    }

    mBundle = bundle;

    if (mBundle)
    {
        connect(mBundle.get(), SIGNAL(transformChanged()), this, SLOT(bundleTransformChanged()));
        connect(mBundle.get(), SIGNAL(bundleChanged()), this, SLOT(bundleChanged()));
    }

    bundleChanged();
}

/**
  * Adds rep assigned actors to the active renderer
  */
void FiberBundleRep::addRepActorsToViewRenderer(View* view)
{
    view->getRenderer()->AddActor(mActor);

    if (mViewportListener)
        mViewportListener->startListen(view->getRenderer());
}

/**
  * Removes rep assigned actors from the rendering pipeline
  */
void FiberBundleRep::removeRepActorsFromViewRenderer(View* view)
{
    view->getRenderer()->RemoveActor(mActor);

    if (mViewportListener)
        mViewportListener->stopListen();
}

/** Called whenever bundle has changed */
void FiberBundleRep::bundleChanged()
{
    vtkPolyDataPtr model = mBundle->getVtkPolyData();

    if (model)
    {
        QColor color = mBundle->getColor();
        mActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());

        if (mTubeFilter)
        {
            /** Create a tube filter for the mesh.
              * This filter enables shading and enhances the otherwise hard to differentiate lines.
              */
            vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
            tubeFilter->SetInput(model);
            tubeFilter->SetRadius(mFiberRadius); //default is .5
            tubeFilter->SetNumberOfSides(mTubeSegments);
            tubeFilter->Update();

            mPolyDataMapper->SetInputConnection(tubeFilter->GetOutputPort());
        }
        else
        {
            {
                vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
                normals->SetInput(model);
                normals->Update();
                model = normals->GetOutput();
            }
            mPolyDataMapper->SetInput(model);
            mActor->SetMapper(mPolyDataMapper);
        }

        mPolyDataMapper->ScalarVisibilityOn();
    }

}

/** Called when a new bundle transformation is available
  */
void FiberBundleRep::bundleTransformChanged()
{
    if (!mBundle) return;

    mActor->SetUserMatrix(mBundle->get_rMd().getVtkMatrix());
}

} // end namespace
