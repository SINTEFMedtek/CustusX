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
void DisplayTextRep::addRepActorsToViewRenderer(View *view)
{
	for(unsigned i =0; i<mDisplayText.size(); ++i)
	{
		view->getRenderer()->AddActor( mDisplayText.at(i)->getActor() );
	}
}

void DisplayTextRep::removeRepActorsFromViewRenderer(View *view)
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
	textRep->textProperty()->SetVerticalJustificationToBottom();
	if (pos[1] > 0.5)
	{
		textRep->textProperty()->SetVerticalJustificationToTop();
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
