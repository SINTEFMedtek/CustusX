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
#include "sscAxesRep.h"

namespace cx
{

typedef boost::shared_ptr<class ToolAxisConnector> ToolAxisConnectorPtr;

class ToolAxisConnector : public QObject
{
	Q_OBJECT
public:
	explicit ToolAxisConnector(ssc::ToolPtr tool);
	ssc::AxesRepPtr getAxis_t();
	ssc::AxesRepPtr getAxis_s();

private slots:
	void transformChangedSlot();
	void visibleSlot();
private:
	ssc::ToolPtr mTool;

	ssc::AxesRepPtr mAxis_t; ///< axis of the tool space
	ssc::AxesRepPtr mAxis_s; /// axis of the tool sensor space
	//New(const QString& uid);
};




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
  virtual void setViewGroup(ViewGroupDataPtr group);

private slots:
  void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
  void toolsAvailableSlot(); ///< add all tools when configured
  void showSlicePlanesActionSlot(bool checked);
  void fillSlicePlanesActionSlot(bool checked);
  void showAxesActionSlot(bool checked);
  void showManualToolSlot(bool visible);
  void resetCameraActionSlot();
  void activeImageChangedSlot();

private:
  virtual void appendToContextMenu(QMenu& contextMenu);
  void updateView();

  virtual void imageAdded(ssc::ImagePtr image);
  virtual void meshAdded(ssc::MeshPtr mesh);
  virtual void imageRemoved(const QString& uid);
  virtual void meshRemoved(const QString& uid);

  typedef  std::map<QString, ssc::VolumetricRepPtr> VolumetricRepMap;
  typedef  std::map<QString, ssc::GeometricRepPtr> GeometricRepMap;
  VolumetricRepMap mVolumetricReps;
  LandmarkRepPtr mLandmarkRep;
  ssc::ProbeRepPtr mProbeRep;
  GeometricRepMap mGeometricReps;
  ssc::DisplayTextRepPtr mPlaneTypeText;
  ssc::DisplayTextRepPtr mDataNameText;
  std::map<QString, ssc::ToolRep3DPtr> mToolReps;
  std::map<QString, ToolAxisConnectorPtr> mToolAxis;
  ssc::AxesRepPtr mRefSpaceAxisRep;
  std::map<QString, ssc::AxesRepPtr> mDataSpaceAxisRep;

  bool mShowAxes; ///< show 3D axes reps for all tools and ref space

  ssc::SlicePlanes3DRepPtr mSlicePlanes3DRep;
  QPointer<ssc::View> mView;
};
typedef boost::shared_ptr<ViewWrapper3D> ViewWrapper3DPtr;

} // namespace cx

#endif /* CXVIEWWRAPPER3D_H_ */
