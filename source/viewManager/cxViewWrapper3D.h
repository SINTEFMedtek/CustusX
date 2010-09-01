/*
 * cxViewWrapper3D.h
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */
#ifndef CXVIEWWRAPPER3D_H_
#define CXVIEWWRAPPER3D_H_

#include <vector>
#include <QPointer>
#include <QObject>
#include "sscData.h"
#include "sscDefinitions.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "cxForwardDeclarations.h"
#include "cxInteractiveCropper.h"

//typedef vtkSmartPointer<class vtkBoxWidget> vtkBoxWidgetPtr;
//typedef vtkSmartPointer<class vtkBoxWidget2> vtkBoxWidget2Ptr;
//typedef vtkSmartPointer<class vtkBoxRepresentation> vtkBoxRepresentationPtr;
//typedef vtkSmartPointer<class vtkTransform> vtkTransformPtr;


typedef vtkSmartPointer<class vtkBoxWidget> vtkBoxWidgetPtr;
typedef vtkSmartPointer<class vtkBoxWidget2> vtkBoxWidget2Ptr;
typedef vtkSmartPointer<class vtkBoxRepresentation> vtkBoxRepresentationPtr;

namespace cx
{


/** Wrapper for a View3D.
 *  Handles the connections between specific reps and the view.
 *
 */
class ViewWrapper3D : public ViewWrapper
{
  Q_OBJECT
public:
  ViewWrapper3D(int startIndex, ssc::View* view);
  virtual ~ViewWrapper3D();
  virtual void addImage(ssc::ImagePtr image);
  virtual void addMesh(ssc::MeshPtr mesh);
  virtual void removeMesh(ssc::MeshPtr mesh);
  virtual std::vector<ssc::ImagePtr> getImages() const;
  virtual ssc::MeshPtr getMesh() const;
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);
  virtual ssc::View* getView();
  virtual double getZoom2D(){return -1.0;};
  virtual void setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy);

private slots:
  void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
  void toolsAvailableSlot(); ///< add all tools when configured
  void showSlicePlanesActionSlot(bool checked);
  void fillSlicePlanesActionSlot(bool checked);
  void resetCameraActionSlot();

private:
  virtual void appendToContextMenu(QMenu& contextMenu);
  void updateView();

  vtkBoxRepresentationPtr mBoxRep;
  vtkBoxWidget2Ptr mBoxWidget2;
  vtkBoxWidgetPtr mBoxWidget;

  typedef  std::map<std::string, ssc::VolumetricRepPtr> VolumetricRepMap;
  VolumetricRepMap mVolumetricReps;
  LandmarkRepPtr mLandmarkRep;
  ssc::ProbeRepPtr mProbeRep;
  ssc::GeometricRepPtr mGeometricRep;
  //std::map<std::string, ssc::GeometricRepPtr> mGeometricReps;//TODO: Replace mGeometricRep with this
  ssc::DisplayTextRepPtr mPlaneTypeText;
  ssc::DisplayTextRepPtr mDataNameText;
  std::map<std::string, ssc::ToolRep3DPtr> mToolReps;
  ssc::SlicePlanes3DRepPtr mSlicePlanes3DRep;

  std::vector<ssc::ImagePtr> mImage;
  //std::vector<ssc::MeshPtr> mMeshes;
  ssc::MeshPtr mMesh;
  QPointer<ssc::View> mView;
};
typedef boost::shared_ptr<ViewWrapper3D> ViewWrapper3DPtr;

} // namespace cx

#endif /* CXVIEWWRAPPER3D_H_ */
