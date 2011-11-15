#include "sscDisplayTextRep.h"

#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>

#include "sscView.h"
#include "sscVtkHelperClasses.h"
#include "sscTypeConversions.h"

namespace ssc
{

DisplayTextRep::DisplayTextRep(const QString& uid, const QString& name=""):
	RepImpl(uid,name)
{
}

DisplayTextRep::~DisplayTextRep()
{

}
DisplayTextRepPtr DisplayTextRep::New(const QString& uid, const QString& name="")
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

/**Add a text with give RGB color at pos.
 * pos is in normalized view space.
 */
TextDisplayPtr DisplayTextRep::addText(const Vector3D& color, const QString& text, const Vector3D& pos, int maxWidth, vtkViewport *vp)
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

	if (maxWidth != 0 && vp)
	{
		textRep->setMaxWidth(maxWidth, vp);
	}
	return textRep;
}

/**Set a text previously set with addText.
 *
 */
void DisplayTextRep::setText(unsigned i, const QString& text)
{
	if (i<mDisplayText.size())
		mDisplayText[i]->updateText(text);	
}

/**set the color in all existing texts
 *
 */
void DisplayTextRep::setColor(const Vector3D& color )
{
	Vector3D c = color;
	for(unsigned i =0; i<mDisplayText.size(); ++i)
	{
		mDisplayText.at(i)->textProperty()->SetColor(c.begin()) ;
	}
	
}

void DisplayTextRep::setFontSize(int size)
{
  for(unsigned i =0; i<mDisplayText.size(); ++i)
  {
    mDisplayText.at(i)->textProperty()->SetFontSize(size) ;
  }
}




//-----------------------------------------------------------------------
}//end namespace
//-----------------------------------------------------------------------
