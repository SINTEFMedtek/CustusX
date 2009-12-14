#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkObjectFactory.h>

#include "sscView.h"
#include "sscOrientationAnnotationRep.h"


// --------------------------------------------------------
namespace ssc
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

OrientationAnnotationRep::OrientationAnnotationRep( const std::string& uid, const std::string& name) :
RepImpl(uid, name)
{
}
OrientationAnnotationRepPtr OrientationAnnotationRep::New(const std::string& uid,const std::string& name)
{
	OrientationAnnotationRepPtr retval(new OrientationAnnotationRep(uid,name));
	retval->mSelf = retval;
	return retval;
}

OrientationAnnotationRep::~OrientationAnnotationRep()
{

}
void OrientationAnnotationRep::setPlaneType(PLANE_TYPE type )
{
	if (ptSAGITTAL == type)
	{
		mNorthAnnotation = "S";
		mSouthAnnotation = "I";
		mEastAnnotation = "P";
		mWestAnnotation = "A";
	}
	if (ptCORONAL == type)
	{
		mNorthAnnotation = "S";
		mSouthAnnotation = "I" ;
		mEastAnnotation = "R" ;
		mWestAnnotation = "L";
	}
	if (ptAXIAL == type)
	{
		mNorthAnnotation = "A";
		mSouthAnnotation = "P";
		mEastAnnotation = "R";
		mWestAnnotation = "L";
	}
}
void OrientationAnnotationRep::addRepActorsToViewRenderer(ssc::View* view)
{
	createAnnotation( view->getRenderer() );
}
void OrientationAnnotationRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	view->getRenderer()->RemoveActor(mOrientation);
}

void OrientationAnnotationRep::createAnnotation(vtkRendererPtr render)
{
	mOrientation = OrientationAnnotationPtr::New();
	mOrientation->SetNonlinearFontScaleFactor (0.35 );
	mOrientation->GetTextProperty()->SetColor(0.7372, 0.815, 0.6039 );
	mOrientation->SetText(0, mEastAnnotation.c_str() );
	mOrientation->SetText(1, mNorthAnnotation.c_str() );
	mOrientation->SetText(2, mWestAnnotation.c_str() );
	mOrientation->SetText(3, mSouthAnnotation.c_str() );
	render->AddActor(mOrientation);

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


// --------------------------------------------------------
} //end namespace
// --------------------------------------------------------


