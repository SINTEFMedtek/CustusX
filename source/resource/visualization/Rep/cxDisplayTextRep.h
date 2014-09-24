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


#ifndef CXDISPLAYTEXTREP_H_
#define CXDISPLAYTEXTREP_H_

#include "cxResourceVisualizationExport.h"

#include <vector>
#include "cxRepImpl.h"
#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"
#include "cxBoundingBox3D.h"
class QColor;

namespace cx
{
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;

typedef boost::shared_ptr<class DisplayTextRep> DisplayTextRepPtr;

/**\brief Display a number of text strings.
 *
 * Display a number of text strings at fixed positions in the view.
 * The positioning is done in normalized view space, i.e. the text
 * is placed relative to the view and not the world.
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 * \ingroup cx_resource_visualization_rep3D
 * \ingroup cx_resource_visualization_repvideo
 */
class cxResourceVisualization_EXPORT DisplayTextRep : public RepImpl
{
public:
	static DisplayTextRepPtr New(const QString& uid, const QString& name);
	virtual QString getType() const{ return "vm::DisplayTextRep";}
	TextDisplayPtr addText(const QColor& color, const QString& text, const Vector3D& pos, int maxWidth=0, vtkViewport *vp=NULL);
	void setText(unsigned i, const QString& text);
	void setColor(const QColor& color );
	void setFontSize(int size);
	virtual ~DisplayTextRep();

protected:
	DisplayTextRep(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

	std::vector<TextDisplayPtr> mDisplayText;
};

}//end namespace

#endif /*CXDISPLAYTEXTREP_H_*/
