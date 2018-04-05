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


/** Follows vtk spec.
 */
OrientationAnnotation* OrientationAnnotation::New()
{
	vtkObject* ret = vtkObjectFactory::CreateInstance("OrientationAnnotation");
	if (ret)
	{
		return static_cast<OrientationAnnotation*>(ret);
	}
	return new OrientationAnnotation;
}

OrientationAnnotation::OrientationAnnotation()
{

}

OrientationAnnotation::~OrientationAnnotation()
{
}

void OrientationAnnotation::SetTextActorsPosition(int vsize[2])
{
	//Logger::log("nav.log","set text position");
	this->TextActor[2]->SetPosition(5, vsize[1]/2);
	this->TextActor[3]->SetPosition(vsize[0]/2, 7);
	this->TextActor[0]->SetPosition(vsize[0]-7, vsize[1]/2);
	this->TextActor[1]->SetPosition(vsize[0]/2, vsize[1]-7);
}
void OrientationAnnotation::SetTextActorsJustification()
{
	vtkTextProperty* tprop = this->TextMapper[2]->GetTextProperty();
	tprop->SetJustificationToLeft();
	tprop->SetVerticalJustificationToCentered();

	tprop = this->TextMapper[3]->GetTextProperty();
	tprop->SetJustificationToCentered();
	tprop->SetVerticalJustificationToBottom();

	tprop = this->TextMapper[0]->GetTextProperty();
	tprop->SetJustificationToRight();
	tprop->SetVerticalJustificationToCentered();

	tprop = this->TextMapper[1]->GetTextProperty();
	tprop->SetJustificationToCentered();
	tprop->SetVerticalJustificationToTop();
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


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
		mOrientation = OrientationAnnotationPtr::New();
		mOrientation->SetNonlinearFontScaleFactor (0.35 );
		mOrientation->GetTextProperty()->SetColor(0.7372, 0.815, 0.6039 );
	}
	mOrientation->SetText(0, cstring_cast(mEastAnnotation) );
	mOrientation->SetText(1, cstring_cast(mNorthAnnotation) );
	mOrientation->SetText(2, cstring_cast(mWestAnnotation) );
	mOrientation->SetText(3, cstring_cast(mSouthAnnotation) );
}


// --------------------------------------------------------
} //end namespace
// --------------------------------------------------------


