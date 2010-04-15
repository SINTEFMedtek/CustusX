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
#include "cxForwardDeclarations.h"
#include "sscData.h"
#include "sscDefinitions.h"
#include "cxViewWrapper.h"

namespace cx
{

//std::string planeToString(ssc::PLANE_TYPE val);


/** Wrapper for a View2D.
 *  Handles the connections between specific reps and the view.
 *
 */
class ViewWrapper2D : public ViewWrapper
{
  Q_OBJECT
public:
  ViewWrapper2D(ssc::View* view);
  virtual void initializePlane(ssc::PLANE_TYPE plane);
  virtual void setImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual ssc::View* getView();
  virtual void setZoom2D(double zoomFactor);
  virtual double getZoom2D();

private slots:
  void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
  void viewportChanged();
  void showSlot();
  void mousePressSlot(QMouseEvent* event);
  void mouseWheelSlot(QWheelEvent* event);

private:
  virtual void appendToContextMenu(QMenu& contextMenu);
  virtual void checkFromContextMenu(QAction* theAction, QActionGroup* theActionGroup);
  void addReps();
  ssc::DoubleBoundingBox3D getViewport() const;
  ssc::Transform3D get_vpMs() const;
  ssc::Vector3D qvp2vp(QPoint pos_qvp);
  void moveAxisPos(ssc::Vector3D click_vp);

  ssc::ORIENTATION_TYPE getOrientationType() const;
  void changeOrientationType(ssc::ORIENTATION_TYPE type);

  ssc::Vector3D displayToWorld(ssc::Vector3D p_d) const;
  ssc::Vector3D viewToDisplay(ssc::Vector3D p_v) const;

  double mZoomFactor;
//  ssc::PLANE_TYPE mPlaneType;
  ssc::SliceProxyPtr mSliceProxy;
  ssc::SliceRepSWPtr mSliceRep;
  ssc::ToolRep2DPtr mToolRep2D;
  ssc::OrientationAnnotationRepPtr mOrientationAnnotationRep;
  ssc::DisplayTextRepPtr mPlaneTypeText;
  ssc::DisplayTextRepPtr mDataNameText;
  ssc::View* mView;

  QActionGroup* mOrientationActionGroup;
  QActionGroup* mGlobal2DZoomActionGroup;

};
typedef boost::shared_ptr<ViewWrapper2D> ViewWrapper2DPtr;


} // namespace cx

#endif /* CXVIEWWRAPPER2D_H_ */
