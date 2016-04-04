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

/*
 * cxViewWrapper2D.h
 *
 *  \date Mar 24, 2010
 *      \author christiana
 */
#ifndef CXVIEWWRAPPER2D_H_
#define CXVIEWWRAPPER2D_H_

#include "org_custusx_core_view_Export.h"

#include <vector>
#include <QPointer>
#include "cxForwardDeclarations.h"
#include "cxDefinitions.h"
#include "cxViewWrapper.h"
#include "cxBoundingBox3D.h"
#include "cxTransform3D.h"
#include "sscConfig.h"

class QMouseEvent;
class QWheelEvent;

namespace cx
{
typedef boost::shared_ptr<class OrientationAnnotationSmartRep> OrientationAnnotationSmartRepPtr;
typedef boost::shared_ptr<class ViewFollower> ViewFollowerPtr;
typedef boost::shared_ptr<class Zoom2DHandler> Zoom2DHandlerPtr;
typedef boost::shared_ptr<class DataRepContainer> DataRepContainerPtr;
}


namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */

/** Wrapper for a View2D.
 *  Handles the connections between specific reps and the view.
 *
 */
class org_custusx_core_view_EXPORT ViewWrapper2D: public ViewWrapper
{
Q_OBJECT
public:
	ViewWrapper2D(ViewPtr view, VisServicesPtr backend);
	virtual ~ViewWrapper2D();
	virtual void initializePlane(PLANE_TYPE plane);
	virtual ViewPtr getView();
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy);
	virtual void setViewGroup(ViewGroupDataPtr group);

	virtual void updateView();


    ImagePtr getImageToDisplay();

signals:
	void pointSampled(Vector3D p_r);

protected slots:
	virtual void dataViewPropertiesChangedSlot(QString uid);
	virtual void videoSourcesChangedSlot();

private slots:
	void activeToolChangedSlot(); ///< makes sure the reps are connected to the right tool
	void viewportChanged();
	void showSlot();
	void mousePressSlot(int x, int y, Qt::MouseButtons buttons);
	void mouseMoveSlot(int x, int y, Qt::MouseButtons buttons);
	void mouseWheelSlot(int x, int y, int delta, int orientation, Qt::MouseButtons buttons);
	void settingsChangedSlot(QString key);
	void optionChangedSlot();

protected slots:
	void samplePoint(Vector3D click_vp);
private:
	void moveManualTool(Vector3D vp, Vector3D delta_vp);
	virtual void appendToContextMenu(QMenu& contextMenu);
	void addReps();
	DoubleBoundingBox3D getViewport() const;
	Vector3D qvp2vp(QPoint pos_qvp);
	void setAxisPos(Vector3D click_vp);
	void shiftAxisPos(Vector3D delta_vp);

	ORIENTATION_TYPE getOrientationType() const;

	virtual void imageAdded(ImagePtr image);
    //virtual void imageRemoved(const QString& uid);

	virtual void dataAdded(DataPtr data);
	virtual void dataRemoved(const QString& uid);

    void recreateMultiSlicer();
    void updateItemsFromViewGroup(QString &text);

    void setDataNameText(QString &text);
    void updateDataNameText(QString &text);

    void createAndAddSliceRep();
    void removeAndResetSliceRep();

    bool useGPU2DRendering();
    void createAndAddMultiSliceRep();
    void removeAndResetMultiSliceRep();

	std::vector<ImagePtr> getImagesToView();
	bool isAnyplane();

#ifndef CX_VTK_OPENGL2
	Texture3DSlicerRepPtr mMultiSliceRep;
#endif

	DataRepContainerPtr mDataRepContainer;

	GeometricRep2DPtr mPickerGlyphRep;
	SliceProxyPtr mSliceProxy;
	SliceRepSWPtr mSliceRep;
	ToolRep2DPtr mToolRep2D;
	OrientationAnnotationSmartRepPtr mOrientationAnnotationRep;
	DisplayTextRepPtr mPlaneTypeText;
	DisplayTextRepPtr mDataNameText;
	SlicePlanes3DMarkerIn2DRepPtr mSlicePlanes3DMarker;
	ViewPtr mView;
	ViewFollowerPtr mViewFollower;


	// synchronized data
	Zoom2DHandlerPtr mZoom2D;

	Vector3D mLastClickPos_vp;

	QActionGroup* mOrientationActionGroup;
};
typedef boost::shared_ptr<ViewWrapper2D> ViewWrapper2DPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWWRAPPER2D_H_ */
