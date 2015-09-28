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


#ifndef CXPICKERREP_H_
#define CXPICKERREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include "cxTransform3D.h"
#include "vtkForwardDeclarations.h"

#include "cxGraphicalPrimitives.h"
#include "cxViewportListener.h"
#include "cxForwardDeclarations.h"

class vtkCommand;
typedef vtkSmartPointer<class vtkCallbackCommand> vtkCallbackCommandPtr;

namespace cx
{
typedef boost::shared_ptr<class PickerRep> PickerRepPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class Tool> ToolPtr;

/** \brief Picking of points in an image.
 *
 * When enabled, clicking in the view is interpreted as picking a point on
 * the attached image. The picked point is the projection of the mouse click
 * onto the image. Threshold is used to determine hit.
 *
 * The point is displayed, and the value emitted as a signal.
 *
 * Used by CustusX.
 * Not used by Sonowand.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT PickerRep: public RepImpl
{
Q_OBJECT

public:
	static PickerRepPtr New(PatientModelServicePtr dataManager, const QString& uid=""); ///< for creating new instances
	virtual ~PickerRep(); ///<empty

	virtual QString getType() const; ///< returns a string identifying this class type

	void setTool(ToolPtr tool); ///< set the tool to listen to

	void setEnabled(bool on);
	void setGlyph(MeshPtr glyph);

	void pickLandmark(const Vector3D& clickPosition, vtkRendererPtr renderer); ///< When you don't use the renderwindowinteractor
	Vector3D getPosition() const;
	void setSphereRadius(double radius);

signals:
	void pointPicked(Vector3D p_r); /// the rep sends out a signal when the user picks a point on it
	void dataPicked(QString uid);

public slots:
	void pickLandmarkSlot(vtkObject* renderWindowInteractor); ///< When you use the renderwindowinteractor

protected:
	PickerRep(PatientModelServicePtr dataManager); ///< use New instead
	virtual void addRepActorsToViewRenderer(ViewPtr view); ///< connects to the renderwindowinteractor
	virtual void removeRepActorsFromViewRenderer(ViewPtr view); ///< disconnects from the renderwindowinteractor
	void connectInteractor();
	void disconnectInteractor();
	void scaleSphere();
	virtual void onModifiedStartRender();
	void toolHasChanged();
	static void ProcessEvents(vtkObject* object,
	                                    unsigned long event,
	                                    void* clientdata,
	                                    void* calldata);
	void OnLeftButtonDown();
	void OnLeftButtonUp();
	void OnMouseMove();
	Vector3D getDisplacement();
	Vector3D ComputeDisplayToWorld(Vector3D p_d);
	Vector3D ComputeWorldToDisplay(Vector3D p_w);
	void setGlyphCenter(Vector3D pos);

	bool mEnabled;
	bool mConnected; ///< Interactor connected
	ToolPtr mTool; ///< the connected tool
	Vector3D mPickedPoint; ///< the last point that was successfully sampled from intersection with an image
	double mSphereRadius;
	MeshPtr mGlyph;
	GeometricRepPtr mGlyphRep;
	Vector3D mClickedPoint;
	bool mIsDragging;

	bool mSnapToSurface; ///< if set, clicking on a Data surface will pick that point

	GraphicalPoint3DPtr mGraphicalPoint;
	ViewportListenerPtr mViewportListener;
	vtkCallbackCommandPtr mCallbackCommand;
	PatientModelServicePtr mDataManager;
};

typedef boost::shared_ptr<PickerRep> PickerRepPtr;

}//namespace
#endif /* CXPICKERREP_H_ */
