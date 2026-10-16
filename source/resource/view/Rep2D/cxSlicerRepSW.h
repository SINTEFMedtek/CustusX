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
#include <memory>

#include "cxRepImpl.h"

#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"

namespace cx
{
typedef std::shared_ptr<class SliceProxyInterface> SliceProxyInterfacePtr;
typedef std::shared_ptr<class Image2DProxy> Image2DProxyPtr;


/**\brief Display an image slice in 2D.
 *
 * The image is sliced using a SliceProxy. Only a single
 * volume can be used. A 2D image in the slice plane, e.g. a video stream in a view
 * following the probe, is drawn directly with its transform instead of being sliced.
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
	void setSliceProxy(SliceProxyInterfacePtr slicer);
	QString getImageUid()const;
	void update();
	virtual void printSelf(std::ostream & os, Indent indent);
	static bool isInSlicePlane(Transform3D sMd, Vector3D center_d); ///< A 2D image with this transform lies in the slice plane
	bool isDrawnDirectly() const;

	static constexpr double kMaxAngleFromSlicePlane_rad = 0.0087; ///< 0.5 degrees
	static constexpr double kMaxDistanceFromSlicePlane_mm = 0.5;

protected:
	SliceRepSW();
	virtual void addRepActorsToViewRenderer(ViewPtr view) ;
	virtual void removeRepActorsFromViewRenderer(ViewPtr view) ;

private slots:
	void updateDrawingMethod();

private:
	vtkImageSlicePtr getShownSlice() const;
	SlicedImageProxyPtr mImageSlicer;
	vtkImageSlicePtr mImageSlice;
	Image2DProxyPtr mImage2D; ///< draws a 2D image directly, when it is in the slice plane
	SliceProxyInterfacePtr mSlicer;
	bool mDrawDirectly;
	vtkRendererPtr mRenderer;
};

}// namespace cx

#endif /*CXSLICERREPSW_H_*/
