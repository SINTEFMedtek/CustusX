/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxOrientationAnnotationRep.h"

#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkObjectFactory.h>

#include "cxView.h"
#include "cxSliceProxy.h"
#include "cxVtkHelperClasses.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"


// --------------------------------------------------------
namespace cx
// --------------------------------------------------------
{

OrientationAnnotationRep::OrientationAnnotationRep(PatientModelServicePtr dataManager) :
	RepImpl(),
	mDataManager(dataManager)
{
	mPlane = ptCOUNT;
	connect(mDataManager.get(), SIGNAL(clinicalApplicationChanged()), this, SLOT(clinicalApplicationChangedSlot()));
}

OrientationAnnotationRepPtr OrientationAnnotationRep::New(PatientModelServicePtr dataManager, const QString& uid)
{
	return wrap_new(new OrientationAnnotationRep(dataManager), uid);
}

OrientationAnnotationRep::~OrientationAnnotationRep()
{

}

void OrientationAnnotationRep::setVisible(bool visible)
{
	mOrientation->SetVisibility(visible);
}


void OrientationAnnotationRep::clinicalApplicationChangedSlot()
{
	this->setPlaneType(mPlane);
}

void OrientationAnnotationRep::setPlaneType(PLANE_TYPE type)
{
	switch (mDataManager->getClinicalApplication())
	{
	case mdRADIOLOGICAL:
	{
		this->setPlaneTypeRadiology(type);
		break;
	}
	case mdNEUROLOGICAL:
	default:
	{
		this->setPlaneTypeNeurology(type);
		break;
	}
	}

	mPlane = type;
	createAnnotation();
}

void OrientationAnnotationRep::setPlaneTypeNeurology(PLANE_TYPE type)
{
	switch (type)
	{
	case ptSAGITTAL:
	{
		mNorthAnnotation = "S";
		mSouthAnnotation = "I";
		mEastAnnotation = "P";
		mWestAnnotation = "A";
		break;
	}
	case ptCORONAL:
	{
		mNorthAnnotation = "S";
		mSouthAnnotation = "I" ;
		mEastAnnotation = "R" ;
		mWestAnnotation = "L";
		break;
	}
	case ptAXIAL:
	{
		mNorthAnnotation = "A";
		mSouthAnnotation = "P";
		mEastAnnotation = "R";
		mWestAnnotation = "L";
		break;
	}
	default:
	{
		mNorthAnnotation = "";
		mSouthAnnotation = "";
		mEastAnnotation = "";
		mWestAnnotation = "";
	}
	}
}

void OrientationAnnotationRep::setPlaneTypeRadiology(PLANE_TYPE type)
{
	switch (type)
	{
	case ptSAGITTAL:
	{
		mNorthAnnotation = "S";
		mSouthAnnotation = "I";
		mEastAnnotation = "P";
		mWestAnnotation = "A";
		break;
	}
	case ptCORONAL:
	{
		mNorthAnnotation = "S";
		mSouthAnnotation = "I" ;
		mEastAnnotation = "L" ;
		mWestAnnotation = "R";
		break;
	}
	case ptAXIAL:
	{
		mNorthAnnotation = "A";
		mSouthAnnotation = "P";
		mEastAnnotation = "L";
		mWestAnnotation = "R";
		break;
	}
	default:
	{
		mNorthAnnotation = "";
		mSouthAnnotation = "";
		mEastAnnotation = "";
		mWestAnnotation = "";
	}
	}
}

void OrientationAnnotationRep::addRepActorsToViewRenderer(ViewPtr view)
{
	createAnnotation();
	view->getRenderer()->AddActor(mOrientation);
}

void OrientationAnnotationRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(mOrientation);
}

void OrientationAnnotationRep::createAnnotation()
{
	if (!mOrientation)
	{
		mOrientation = vtkCornerAnnotationPtr::New();
		mOrientation->SetNonlinearFontScaleFactor (0.35 );
		mOrientation->GetTextProperty()->SetColor(0.7372, 0.815, 0.6039 );
	}

	mOrientation->SetText(vtkCornerAnnotation::TextPosition::RightEdge, cstring_cast(mEastAnnotation) );
	mOrientation->SetText(vtkCornerAnnotation::TextPosition::UpperEdge, cstring_cast(mNorthAnnotation) );
	mOrientation->SetText(vtkCornerAnnotation::TextPosition::LeftEdge, cstring_cast(mWestAnnotation) );
	mOrientation->SetText(vtkCornerAnnotation::TextPosition::LowerEdge, cstring_cast(mSouthAnnotation) );
}


// --------------------------------------------------------
} //end namespace
// --------------------------------------------------------


