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
 * Both use the same actor, so the drawing order against other reps doesn't change.
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
	static Transform3D moveIntoSlicePlane(Transform3D sMd, Vector3D center_d); ///< Rotate and move a 2D image near the slice plane exactly into it
	bool isDrawnDirectly() const;
	void setDirectDrawingEnabled(bool enabled); ///< false: always slice the image, also a 2D image in the slice plane

	static constexpr double kMaxAngleFromSlicePlane_rad = 0.0087; ///< 0.5 degrees
	static constexpr double kMaxDistanceFromSlicePlane_mm = 0.5;

protected:
	SliceRepSW();
	vtkImageSlicePtr getActor() const;
	virtual void onModifiedStartRender();
	virtual void addRepActorsToViewRenderer(ViewPtr view) ;
	virtual void removeRepActorsFromViewRenderer(ViewPtr view) ;

private slots:
	void drawingInputChanged();

private:
	void updateDrawingMethod();
	void showSlice(bool drawDirectly);
	SlicedImageProxyPtr mImageSlicer;
	vtkImageSlicePtr mImageSlice;
	Image2DProxyPtr mImage2D; ///< the 2D image with its LUT, created when it is first drawn directly
	SliceProxyInterfacePtr mSlicer;
	bool mDrawDirectly;
	bool mDirectDrawingEnabled;
};

}// namespace cx

#endif /*CXSLICERREPSW_H_*/
