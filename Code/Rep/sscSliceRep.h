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

#ifndef SSCSLICEREP_H_
#define SSCSLICEREP_H_

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImagePlaneWidget> vtkImagePlaneWidgetPtr;

#include "sscRepImpl.h"

namespace ssc
{
typedef boost::shared_ptr<class Image> ImagePtr;

typedef boost::shared_ptr<class SliceRep> SliceRepPtr;

/**\brief Representation for one Image.
 * 		  NOT IN USE BY ANYONE!
 *
 * Use this to slice an image.
 *
 * If you want to use this class, test this thoroughly first!!
 *
 * \ingroup sscRep
 * \ingroup sscRep2D
 */
class SliceRep : public RepImpl
{
public:
	virtual ~SliceRep();

	static SliceRepPtr New(const QString& uid);

	virtual QString getType() const { return "ssc::SliceRep"; }
	void setImage(ImagePtr image);
	bool hasImage(ImagePtr image) const;

protected:
	SliceRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

	vtkImagePlaneWidgetPtr mVtkImagePlaneWidget;

	ImagePtr mImage;
};

} // namespace ssc

#endif /*SSCSLICEREP_H_*/
