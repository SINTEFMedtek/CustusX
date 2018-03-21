/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxOrientationAnnotation2DRep.h"
#include "cxOrientationAnnotationRep.h"

#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkObjectFactory.h>
#include <vtkCornerAnnotation.h>

#include "cxView.h"
#include "cxSliceProxy.h"
#include "cxVtkHelperClasses.h"
#include "cxTypeConversions.h"

// --------------------------------------------------------
namespace cx
// --------------------------------------------------------
{

OrientationAnnotationSmartRep::OrientationAnnotationSmartRep() :
	RepImpl()
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



OrientationAnnotationSmartRepPtr OrientationAnnotationSmartRep::New(const QString& uid)
{
	return wrap_new(new OrientationAnnotationSmartRep(), uid);
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

void OrientationAnnotationSmartRep::addRepActorsToViewRenderer(ViewPtr view)
{
	this->transformChangedSlot();
	view->getRenderer()->AddActor(mOrientation);
}

void OrientationAnnotationSmartRep::removeRepActorsFromViewRenderer(ViewPtr view)
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


