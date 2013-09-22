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

#include "sscOrientationAnnotation2DRep.h"
#include "sscOrientationAnnotationRep.h"

#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkObjectFactory.h>
#include <vtkCornerAnnotation.h>

#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscVtkHelperClasses.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"

// --------------------------------------------------------
namespace cx
// --------------------------------------------------------
{

OrientationAnnotationSmartRep::OrientationAnnotationSmartRep(const QString& uid, const QString& name) :
	RepImpl(uid, name)
{
	mAngle = M_PI*60/180;
	mOrientation = OrientationAnnotationPtr::New();
	mOrientation->SetNonlinearFontScaleFactor(0.35);
	mOrientation->GetTextProperty()->SetColor(0.7372, 0.815, 0.6039);

	mDCMDirections_r["P"] = Vector3D( 0, 1, 0); // Posterior
	mDCMDirections_r["A"] = Vector3D( 0,-1, 0); // Anterior
	mDCMDirections_r["R"] = Vector3D(-1, 0, 0); // Right
	mDCMDirections_r["L"] = Vector3D( 1, 0, 0); // Left
	mDCMDirections_r["S"] = Vector3D( 0, 0, 1); // Superior
	mDCMDirections_r["I"] = Vector3D( 0, 0,-1); // Inferior

	mPlaneDirections_s.resize(4);
	mPlaneDirections_s[0] = Vector3D( 1, 0, 0); // East
	mPlaneDirections_s[1] = Vector3D( 0, 1, 0); // North
	mPlaneDirections_s[2] = Vector3D(-1, 0, 0); // West
	mPlaneDirections_s[3] = Vector3D( 0,-1, 0); // South
}



OrientationAnnotationSmartRepPtr OrientationAnnotationSmartRep::New(const QString& uid, const QString& name)
{
	OrientationAnnotationSmartRepPtr retval(new OrientationAnnotationSmartRep(uid, name));
	retval->mSelf = retval;
	return retval;
}

OrientationAnnotationSmartRep::~OrientationAnnotationSmartRep()
{

}

void OrientationAnnotationSmartRep::setVisible(bool visible)
{
	mOrientation->SetVisibility(visible);
}

void OrientationAnnotationSmartRep::setSliceProxy(SliceProxyPtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot()));
	}
	mSlicer = slicer;
	if (mSlicer)
	{
		connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot()));
		this->transformChangedSlot();
	}
}

void OrientationAnnotationSmartRep::transformChangedSlot()
{
	this->createAnnotation();
}

QString OrientationAnnotationSmartRep::determineAnnotation(Vector3D planeDir_s, Transform3D rMs)
{
	Vector3D planeDir_r = rMs.vector(planeDir_s);

	QString text;
	double threshold = cos(mAngle);
//	double threshold = 0.5;

	for (std::map<QString, Vector3D>::iterator iter=mDCMDirections_r.begin(); iter!=mDCMDirections_r.end(); ++iter)
	{
		double w = dot(planeDir_r, iter->second);
		if (w > threshold)
			text += iter->first;
	}

	return text;
//	return "test_"+qstring_cast(planeDir_s);
}

void OrientationAnnotationSmartRep::addRepActorsToViewRenderer(View* view)
{
	this->transformChangedSlot();
	view->getRenderer()->AddActor(mOrientation);
}

void OrientationAnnotationSmartRep::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mOrientation);
}

void OrientationAnnotationSmartRep::ThresholdAngle(double angle)
{
	mAngle = angle;
	this->createAnnotation();
}

double OrientationAnnotationSmartRep::ThresholdAngle() const
{
	return mAngle;
}

void OrientationAnnotationSmartRep::createAnnotation()
{
	if (!mSlicer)
		return;

	Transform3D rMs = mSlicer->get_sMr().inv();

	// update texts
	for (unsigned int i=0; i<mPlaneDirections_s.size(); ++i)
	{
		QString text = this->determineAnnotation(mPlaneDirections_s[i], rMs);
		mOrientation->SetText(i, cstring_cast(text));
	}
}


// --------------------------------------------------------
} //end namespace
// --------------------------------------------------------


