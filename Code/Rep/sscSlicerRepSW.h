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

#ifndef SSCSLICERREPSW_H_
#define SSCSLICERREPSW_H_

#include "sscRepImpl.h"

#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{

/**\brief Display an image slice in 2D.
 *
 * The image is sliced using a SliceProxy. Only a single
 * volume can be used.
 *
 * Used by CustusX.
 *
 * \sa SlicedImageProxy SliceProxy
 *
 * \ingroup sscRep
 * \ingroup sscRep2D
 */
class SliceRepSW : public RepImpl
{
	Q_OBJECT
public:
	static SliceRepSWPtr New(const QString& uid);
	virtual ~SliceRepSW();
	virtual QString getType() const { return "ssc::SliceRepSW"; };
	bool hasImage(ImagePtr image) const;
	void setImage(ImagePtr image);
	ImagePtr getImage();
	vtkImageActorPtr getActor();
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	QString getImageUid()const;
	void update();
	virtual void printSelf(std::ostream & os, Indent indent);

protected:
	SliceRepSW(const QString& uid);
	virtual void addRepActorsToViewRenderer(View* view) ;
	virtual void removeRepActorsFromViewRenderer(View *view) ;

private:
	SlicedImageProxyPtr mImageSlicer;
	vtkImageActorPtr mImageActor;
};

}// namespace ssc

#endif /*SSCSLICERREPSW_H_*/
