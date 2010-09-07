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

  virtual void imageAdded(ssc::ImagePtr image);
  virtual void meshAdded(ssc::MeshPtr mesh);
  virtual void imageRemoved(const QString& uid);
  virtual void meshRemoved(const QString& uid);

  typedef  std::map<std::string, ssc::VolumetricRepPtr> VolumetricRepMap;
  typedef  std::map<std::string, ssc::GeometricRepPtr> GeometricRepMap;
  VolumetricRepMap mVolumetricReps;
  LandmarkRepPtr mLandmarkRep;
  ssc::ProbeRepPtr mProbeRep;
  GeometricRepMap mGeometricReps;
  ssc::DisplayTextRepPtr mPlaneTypeText;
  ssc::DisplayTextRepPtr mDataNameText;
  std::map<std::string, ssc::ToolRep3DPtr> mToolReps;
  ssc::SlicePlanes3DRepPtr mSlicePlanes3DRep;
  QPointer<ssc::View> mView;
};
typedef boost::shared_ptr<ViewWrapper3D> ViewWrapper3DPtr;

} // namespace cx

#endif /* CXVIEWWRAPPER3D_H_ */
