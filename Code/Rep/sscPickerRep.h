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

namespace ssc
{
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
class PickerRep : public RepImpl
{
	Q_OBJECT

public:
	static PickerRepPtr New(const QString& uid, const QString& name = ""); ///< for creating new instances
	virtual ~PickerRep(); ///<empty

	virtual QString getType() const; ///< returns a string identifying this class type

	int getThreshold(); ///< gets the picking threshold
	void setImage(ImagePtr image); ///< set which image points should be picked from
	ImagePtr getImage();
	void setResolution(const int resolution); ///< sets the resolution of the probing ray
	void setTool(ToolPtr tool); ///< set the tool to listen to

	void setEnabled(bool on);

	Vector3D pickLandmark(const Vector3D& clickPosition, vtkRendererPtr renderer); ///< When you don't use the renderwindowinteractor
	Vector3D getPosition() const;
	void setSphereRadius(double radius);

signals:
	void pointPicked(ssc::Vector3D p_r); /// the rep sends out a signal when the user picks a point on it

public slots:
	void pickLandmarkSlot(vtkObject* renderWindowInteractor); ///< When you use the renderwindowinteractor
	void setThresholdSlot(const int threshold); ///< sets the threshold for picking the point on the volumes surface

protected slots:
  void receiveTransforms(Transform3D prMt, double timestamp); ///< receive transforms from the connected tool

protected:
  PickerRep(const QString& uid, const QString& name=""); ///< use New instead
	virtual void addRepActorsToViewRenderer(View* view); ///< connects to the renderwindowinteractor
	virtual void removeRepActorsFromViewRenderer(View* view); ///< disconnects from the renderwindowinteractor
	vtkRendererPtr getRendererFromRenderWindow(vtkRenderWindowInteractor& iren); ///< tries to get a renderer from the given renderwindowinteractor
	bool intersectData(Vector3D p0, Vector3D p1, Vector3D& intersection); ///< Find the intersection between the probe line and the image.
	void connectInteractor();
	void disconnectInteractor();
	void scaleSphere();

	View* mView;
	bool mEnabled;
	bool mConnected;
	ImagePtr mImage; ///< the image to pick points from
	ToolPtr mTool; ///< the connected tool
	int mThreshold; ///< used to picked the point together with the probefilter, default=25
	int mResolution; ///< used to divide the probing ray into pieces, default=1000
	Vector3D mPickedPoint; ///< the last point that was successfully sampled from intersection with an image
	double mSphereRadius;
	vtkEventQtSlotConnectPtr mConnections; ///< used to sending signals and events between vtk and qt

	ssc::GraphicalPoint3DPtr mGraphicalPoint;
	ssc::ViewportListenerPtr mViewportListener;
};

typedef boost::shared_ptr<PickerRep> PickerRepPtr;

}//namespace
#endif /* SSCPICKERREP_H_ */
