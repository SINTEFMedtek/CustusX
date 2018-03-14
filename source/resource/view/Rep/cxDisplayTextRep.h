/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 * \ingroup cx_resource_view_rep3D
 * \ingroup cx_resource_view_repvideo
 */
class cxResourceVisualization_EXPORT DisplayTextRep : public RepImpl
{
public:
	static DisplayTextRepPtr New(const QString& uid="");
	virtual QString getType() const{ return "vm::DisplayTextRep";}
	TextDisplayPtr addText(const QColor& color, const QString& text, const Vector3D& pos, int maxWidth=0, vtkViewport *vp=NULL);
	void setText(unsigned i, const QString& text);
	void setColor(const QColor& color );
	void setFontSize(int size);
	virtual ~DisplayTextRep();

protected:
	DisplayTextRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

	std::vector<TextDisplayPtr> mDisplayText;
};

}//end namespace

#endif /*CXDISPLAYTEXTREP_H_*/
