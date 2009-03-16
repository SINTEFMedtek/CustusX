#ifndef CXVOLUMETRICREP_H_
#define CXVOLUMETRICREP_H_

#include "sscVolumetricRep.h"

/**
 * cxVolumetricRep.h
 *
 * \brief
 *
 * \date Dec 10, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class vtkObject;
class vtkRenderer;
class vtkActor;
class vtkRenderWindowInteractor;
typedef vtkSmartPointer<vtkActor> vtkActorPtr;
typedef vtkSmartPointer<vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkEventQtSlotConnect> vtkEventQtSlotConnectPtr;
namespace cx
{
class MessageManager;
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;

class VolumetricRep : public ssc::VolumetricRep
{
  Q_OBJECT

public:
  static VolumetricRepPtr New(const std::string& uid, const std::string& name="");

  virtual std::string getType() const { return "cx::VolumeRep"; } ///< returns this classes type
  virtual void setImage(ssc::ImagePtr image);
  void setThreshold(const int threshold); ///< sets the threshold for picking the point on the volumes surface
  void pickSurfacePoint(vtkObject* object, double &x, double &y, double &z); ///< picks a point on the volumes surface
  void makePointPermanent(unsigned int index); ///< calling this will emit a signal with the current point

signals:
  void addPermanentPoint(double x, double y, double z, unsigned int); ///< signal requesting this point be made permanent
  void pointPicked(double x, double y, double z); /// the rep sends out a signal when the user picks a point on it

public slots:
  void pickSurfacePointSlot(vtkObject* object); ///< picks a point on the volumes surface
  void showTemporaryPointSlot(double x, double y, double z); ///< shows a temp actor at a given position

protected:
  VolumetricRep(const std::string& uid, const std::string& name=""); ///< constructor
  vtkSmartPointer<vtkRenderer> getRendererFromRenderWindow(vtkRenderWindowInteractor& iren); ///< tries to get a renderer from the given renderwindowinteractor
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);
  bool doesLandmarkAlreadyExist(double x, double y, double z);

  MessageManager*     mMessageManager;  ///< device for sending messages to the statusbar

  int                 mThreshold;                       ///< used to picked the point together with the probefilter
  double              mCurrentX, mCurrentY, mCurrentZ;  ///< the last point that was successfully sampled
  vtkActorPtr         mPickedPointActor;                ///< the actor showing the last successfully sampled point
  vtkRendererPtr      mCurrentRenderer;

  vtkEventQtSlotConnectPtr mConnections; ///< used to sending signals og evnts between qt and vtk
};
typedef boost::shared_ptr<VolumetricRep> VolumetricRepPtr;
}//namespace cx
#endif /* CXVOLUMETRICREP_H_ */
