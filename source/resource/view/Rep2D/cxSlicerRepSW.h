/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXSLICERREPSW_H_
#define CXSLICERREPSW_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"

#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class SliceProxyInterface> SliceProxyInterfacePtr;


/**\brief Display an image slice in 2D.
 *
 * The image is sliced using a SliceProxy. Only a single
 * volume can be used.
 *
 * Used by CustusX.
 *
 * \sa SlicedImageProxy SliceProxy
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 */
class cxResourceVisualization_EXPORT SliceRepSW : public RepImpl
{
	Q_OBJECT
public:
	static SliceRepSWPtr New(const QString& uid="");
	virtual ~SliceRepSW();
	virtual QString getType() const { return "SliceRepSW"; };
	bool hasImage(ImagePtr image) const;
	void setImage(ImagePtr image);
	ImagePtr getImage();
	vtkImageActorPtr getActor();
	void setSliceProxy(SliceProxyInterfacePtr slicer);
	QString getImageUid()const;
	void update();
	virtual void printSelf(std::ostream & os, Indent indent);

protected:
	SliceRepSW();
	virtual void addRepActorsToViewRenderer(ViewPtr view) ;
	virtual void removeRepActorsFromViewRenderer(ViewPtr view) ;

private:
	SlicedImageProxyPtr mImageSlicer;
	vtkImageActorPtr mImageActor;
};

}// namespace cx

#endif /*CXSLICERREPSW_H_*/
