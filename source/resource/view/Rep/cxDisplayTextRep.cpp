/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
