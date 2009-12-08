#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include "sscDisplayTextRep.h"
#include "sscView.h"

namespace ssc
{

DisplayTextRep::DisplayTextRep(const std::string& uid, const std::string& name=""):
	RepImpl(uid,name)
{
}

DisplayTextRep::~DisplayTextRep()
{

}
DisplayTextRepPtr DisplayTextRep::New(const std::string& uid, const std::string& name="")
{
	DisplayTextRepPtr retval(new DisplayTextRep(uid, name));
	retval->mSelf = retval;
	return retval;	
}
void DisplayTextRep::addRepActorsToViewRenderer(View* view)
{
	for(unsigned i =0; i<mDisplayText.size(); ++i)
	{
		view->getRenderer()->AddActor( mDisplayText.at(i)->getActor() );
	}
}

void DisplayTextRep::removeRepActorsFromViewRenderer(View* view) 
{
	for(unsigned i =0; i<mDisplayText.size(); ++i)
		{
			view->getRenderer()->RemoveActor(mDisplayText.at(i)->getActor() );
		}
}
void DisplayTextRep::addText(const Vector3D& color, const std::string& text, const Vector3D& pos)
{
	Vector3D c = color;
	TextDisplayPtr textRep;
	textRep.reset( new TextDisplay( text, c, 20) );	
	textRep->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	textRep->setPosition(pos);
	textRep->textProperty()->SetJustificationToLeft();
	if( pos[0]>0.5 )
	{
		textRep->textProperty()->SetJustificationToRight();
	}
	//textRep->setCentered();
	mDisplayText.push_back( textRep );

}

void DisplayTextRep::setText(unsigned i, const std::string& text)
{
	if (i<mDisplayText.size())
		mDisplayText[i]->updateText(text);	
}

void DisplayTextRep::setColor(const Vector3D& color )
{
	Vector3D c = color;
	for(unsigned i =0; i<mDisplayText.size(); ++i)
	{
		mDisplayText.at(i)->textProperty()->SetColor(c.begin()) ;
	}
	
}



//-----------------------------------------------------------------------
}//end namespace
//-----------------------------------------------------------------------
