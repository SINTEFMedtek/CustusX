/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxDisplayTextRep.h"

#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>

#include "cxView.h"
#include "cxVtkHelperClasses.h"
#include "cxTypeConversions.h"

namespace cx
{

DisplayTextRep::DisplayTextRep():
	RepImpl("")
{
}

DisplayTextRep::~DisplayTextRep()
{

}
DisplayTextRepPtr DisplayTextRep::New(const QString& uid)
{
	return wrap_new(new DisplayTextRep(), uid);
}
void DisplayTextRep::addRepActorsToViewRenderer(ViewPtr view)
{
	for(unsigned i =0; i<mDisplayText.size(); ++i)
		mDisplayText[i]->setRenderer(view->getRenderer());
}

void DisplayTextRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	for(unsigned i =0; i<mDisplayText.size(); ++i)
		mDisplayText[i]->setRenderer(NULL);
}

/**Add a text with give RGB color at pos.
 * pos is in normalized view space.
 */
TextDisplayPtr DisplayTextRep::addText(const QColor& color, const QString& text, const Vector3D& pos, int maxWidth, vtkViewport *vp)
{
//	Vector3D c = color;
	TextDisplayPtr textRep;
	textRep.reset( new TextDisplay( text, color, 20) );
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

	textRep->setRenderer(this->getRenderer());

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
void DisplayTextRep::setColor(const QColor& color )
{
	for(unsigned i =0; i<mDisplayText.size(); ++i)
	{
		mDisplayText.at(i)->textProperty()->SetColor(getColorAsVector3D(color).begin()) ;
	}

}

void DisplayTextRep::setFontSize(int size)
{
	for(unsigned i =0; i<mDisplayText.size(); ++i)
	{
		mDisplayText.at(i)->textProperty()->SetFontSize(size) ;
		if (size != mDisplayText[i]->textProperty()->GetFontSize())
			mDisplayText.at(i)->textProperty()->SetFontSize(size) ;
	}
}

//-----------------------------------------------------------------------
}//end namespace
//-----------------------------------------------------------------------
