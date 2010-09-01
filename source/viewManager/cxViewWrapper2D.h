/*
 * cxViewWrapper2D.h
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */
#ifndef CXVIEWWRAPPER2D_H_
#define CXVIEWWRAPPER2D_H_

#include <vector>
#include <QtGui>
#include <QPointer>
#include "cxForwardDeclarations.h"
#include "sscData.h"
#include "sscDefinitions.h"
#include "sscMesh.h"
#include "cxViewWrapper.h"

namespace cx
{

/** Wrapper for a View2D.
 *  Handles the connections between specific reps and the view.
 *
 */
class ViewWrapper2D : public ViewWrapper
{
  Q_OBJECT
public:
  ViewWrapper2D(ssc::View* view);
  virtual ~ViewWrapper2D();
  virtual void initializePlane(ssc::PLANE_TYPE plane);
  virtual void addImage(ssc::ImagePtr image);
  virtual void addMesh(ssc::MeshPtr mesh) {/*TODO */ std::cout << "This is not implemented" << std::endl;};
  std::vector<ssc::ImagePtr> getImages() const;
  //virtual ssc::MeshPtr getMesh() const {/*TODO*/ std::cout << "This is not implemented" << std::endl; return ssc::MeshPtr(new ssc::Mesh("dummy"));};
  virtual std::vector<ssc::MeshPtr> getMeshes() const { return std::vector<ssc::MeshPtr>(); }
  virtual void removeImage(ssc::ImagePtr image);
  virtual void removeMesh(ssc::MeshPtr mesh);
  virtual ssc::View* getView();
  virtual void setZoom2D(SyncedValuePtr value);
  virtual void setOrientationMode(SyncedValuePtr value);
  virtual void setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy);

private slots:
  void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
  void viewportChanged();
  void showSlot();
  void mousePressSlot(QMouseEvent* event);
  void mouseMoveSlot(QMouseEvent* event);
  void mouseWheelSlot(QWheelEvent* event);
  void orientationActionSlot();
  void global2DZoomActionSlot();
  void orientationModeChanged();


private:
  virtual void setZoomFactor2D(double zoomFactor);
  virtual double getZoomFactor2D() const;
  void updateView();

  virtual void appendToContextMenu(QMenu& contextMenu);
  void addReps();
  ssc::DoubleBoundingBox3D getViewport() const;
  ssc::Transform3D get_vpMs() const;
  ssc::Vector3D qvp2vp(QPoint pos_qvp);
  void moveAxisPos(ssc::Vector3D click_vp);

  ssc::ORIENTATION_TYPE getOrientationType() const;
  void changeOrientationType(ssc::ORIENTATION_TYPE type);

  ssc::Vector3D displayToWorld(ssc::Vector3D p_d) const;
  ssc::Vector3D viewToDisplay(ssc::Vector3D p_v) const;

  ssc::SliceProxyPtr mSliceProxy;
  ssc::SliceRepSWPtr mSliceRep;
  ssc::ToolRep2DPtr mToolRep2D;
  ssc::OrientationAnnotationRepPtr mOrientationAnnotationRep;
  ssc::DisplayTextRepPtr mPlaneTypeText;
  ssc::DisplayTextRepPtr mDataNameText;
  ssc::SlicePlanes3DMarkerIn2DRepPtr mSlicePlanes3DMarker;
  QPointer<ssc::View> mView;
  std::vector<ssc::ImagePtr> mImage;

  // sunchronized data
  SyncedValuePtr mZoom2D;
  SyncedValuePtr mOrientationMode;

  QActionGroup* mOrientationActionGroup;

};
typedef boost::shared_ptr<ViewWrapper2D> ViewWrapper2DPtr;


} // namespace cx

#endif /* CXVIEWWRAPPER2D_H_ */
