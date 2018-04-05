/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
