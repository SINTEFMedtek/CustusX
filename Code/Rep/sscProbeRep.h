#ifndef SSCPROBEREP_H_
#define SSCPROBEREP_H_

#include <boost/shared_ptr.hpp>
#include "vtkSmartPointer.h"
#include "sscRepImpl.h"
#include "sscVector3D.h"

class vtkObject;
class vtkRenderWindowInteractor;

typedef vtkSmartPointer<class vtkActor> vtkActorPtr;
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkLineSource> vtkLineSourcePtr;
typedef vtkSmartPointer<class vtkProbeFilter> vtkProbeFilterPtr;
typedef vtkSmartPointer<class vtkSphereSource> vtkSphereSourcePtr;
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;
typedef vtkSmartPointer<class vtkDataSetAttributes> vtkDataSetAttributesPtr;
typedef vtkSmartPointer<class vtkEventQtSlotConnect> vtkEventQtSlotConnectPtr;
typedef vtkSmartPointer<class vtkRenderWindowInteractor> vtkRenderWindowInteractorPtr;

namespace ssc
{
typedef boost::shared_ptr<class ProbeRep> ProbeRepPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class Vector3D> Vector3DPtr;

/**
 * Representation of points picked in an image.
 */
class ProbeRep : public RepImpl
{
	Q_OBJECT

public:
	static ProbeRepPtr New(const std::string& uid, const std::string& name=""); ///<
	virtual ~ProbeRep(); ///<

	virtual std::string getType() const; ///<

	void setImage(ImagePtr image); ///< set which image points should be picked from
	void setThreshold(const int threshold); ///< sets the threshold for picking the point on the volumes surface
	void setResolution(const int resolution); ///< sets the resolution of the probing ray
	//void useRenderWindowInteractor(bool use);

	Vector3D pickLandmark(const Vector3D& clickPosition, vtkRendererPtr renderer); ///< When you don't use the renderwindowinteractor
	void makeLandmarkPermanent(unsigned int index); ///< sends out a signal to the image to make the picked landmark permanent

signals:
	void addPermanentPoint(double x, double y, double z, unsigned int); ///< signal requesting this point be made permanent
	void pointPicked(double x, double y, double z); /// the rep sends out a signal when the user picks a point on it

public slots:
	void pickLandmarkSlot(vtkObject* renderWindowInteractor); ///< When you use the renderwindowinteractor
	void showTemporaryPointSlot(double x, double y, double z); ///< shows a temp actor at a given position

protected:
	ProbeRep(const std::string& uid, const std::string& name=""); ///<
	virtual void addRepActorsToViewRenderer(View* view); ///<
	virtual void removeRepActorsFromViewRenderer(View* view); ///<
	vtkRendererPtr getRendererFromRenderWindow(vtkRenderWindowInteractor& iren); ///< tries to get a renderer from the given renderwindowinteractor
	bool intersectData(Vector3D p0, Vector3D p1, Vector3D& intersection); ///< Find the intersection between the probe line and the image.
	bool snapToExistingPoint(const Vector3D& p0, const Vector3D& p1, Vector3D& bestPoint); ///< if there is a landmark close by, use that instead

	ImagePtr            mImage;                           ///< the image to pick points from
	int                 mThreshold;                       ///< used to picked the point together with the probefilter, default=25
	int                 mResolution;                      ///< used to divide the probing ray into pieces, default=1000
	Vector3D            mPickedPoint;                     ///< the last point that was successfully sampled from intersection with an image
	vtkActorPtr         mPickedPointActor;                ///< the actor showing the last successfully sampled point
	vtkRendererPtr      mCurrentRenderer;                 ///< TODO

	vtkEventQtSlotConnectPtr mConnections;                ///< used to sending signals and events between vtk and qt
};

typedef boost::shared_ptr<ProbeRep> ProbeRepPtr;
}//namespace
#endif /* SSCPROBEREP_H_ */
