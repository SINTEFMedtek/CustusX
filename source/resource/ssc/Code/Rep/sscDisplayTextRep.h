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

#ifndef SSCDISPLAYTEXTREP_H_
#define SSCDISPLAYTEXTREP_H_

#include <vector>
#include "sscRepImpl.h"
#include "sscVector3D.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;

typedef boost::shared_ptr<class DisplayTextRep> DisplayTextRepPtr;

/**\brief Display a number of text strings.
 *
 * Display a number of text strings at fixed positions in the view.
 * The positioning is done in normalized view space, i.e. the text
 * is placed relative to the view and not the world.
 *
 * \ingroup sscRep
 * \ingroup sscRep2D
 * \ingroup sscRep3D
 * \ingroup sscRepVideo
 */
class DisplayTextRep : public ssc::RepImpl
{
public:
	static DisplayTextRepPtr New(const QString& uid, const QString& name);
	virtual QString getType() const{ return "vm::DisplayTextRep";}
	TextDisplayPtr addText(const Vector3D& color, const QString& text, const Vector3D& pos, int maxWidth=0, vtkViewport *vp=NULL);
	void setText(unsigned i, const QString& text);
	void setColor(const Vector3D& color );
	void setFontSize(int size);
	virtual ~DisplayTextRep();

protected:
	DisplayTextRep(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);

	std::vector<TextDisplayPtr> mDisplayText;
};

}//end namespace

#endif /*SSCDISPLAYTEXTREP_H_*/
