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

#ifndef SSCPICKERREP_H_
#define SSCPICKERREP_H_

#include "sscRepImpl.h"
#include "sscTransform3D.h"
#include "vtkForwardDeclarations.h"

#include "sscGraphicalPrimitives.h"
#include "sscViewportListener.h"
#include "sscForwardDeclarations.h"

class vtkCommand;
typedef vtkSmartPointer<class vtkCallbackCommand> vtkCallbackCommandPtr;

namespace cx
{
class DataManager;
typedef boost::shared_ptr<class PickerRep> PickerRepPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class Tool> ToolPtr;

/**\brief Picking of points in an image.
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
 * \ingroup sscRep
 * \ingroup sscRep3D
 */
class PickerRep: public RepImpl
{
Q_OBJECT

public:
	static PickerRepPtr New(DataServicePtr dataManager, const QString& uid, const QString& name = ""); ///< for creating new instances
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
	PickerRep(DataServicePtr dataManager, const QString& uid, const QString &name); ///< use New instead
	virtual void addRepActorsToViewRenderer(View *view); ///< connects to the renderwindowinteractor
	virtual void removeRepActorsFromViewRenderer(View *view); ///< disconnects from the renderwindowinteractor
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

	View *mView;
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
	DataServicePtr mDataManager;
};

typedef boost::shared_ptr<PickerRep> PickerRepPtr;

}//namespace
#endif /* SSCPICKERREP_H_ */
