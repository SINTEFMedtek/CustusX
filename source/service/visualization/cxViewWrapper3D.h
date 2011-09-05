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
#include "sscAxesRep.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "cxForwardDeclarations.h"

typedef vtkSmartPointer<class vtkAnnotatedCubeActor> vtkAnnotatedCubeActorPtr;
typedef vtkSmartPointer<class vtkOrientationMarkerWidget> vtkOrientationMarkerWidgetPtr;

namespace cx
{
typedef boost::shared_ptr<class ImageLandmarkRep> ImageLandmarkRepPtr;
typedef boost::shared_ptr<class PatientLandmarkRep> PatientLandmarkRepPtr;

typedef boost::shared_ptr<class ToolAxisConnector> ToolAxisConnectorPtr;

 enum STEREOTYPE
{
  stFRAME_SEQUENTIAL,
  stINTERLACED,
  stDRESDEN,
  stRED_BLUE
};

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
  virtual ssc::View* getView();
  virtual double getZoom2D(){return -1.0;};
  virtual void setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy);
  virtual void setViewGroup(ViewGroupDataPtr group);
  void setStereoType(int type);

private slots:
  void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
  void toolsAvailableSlot(); ///< add all tools when configured
  void showSlicePlanesActionSlot(bool checked);
  void fillSlicePlanesActionSlot(bool checked);
  void showAxesActionSlot(bool checked);
  void showManualToolSlot(bool visible);
  void resetCameraActionSlot();
  void activeImageChangedSlot();
  void showRefToolSlot(bool checked);
  void showToolPathSlot(bool checked);
  void probeRepPointPickedSlot(ssc::Vector3D p_r);
  void centerImageActionSlot();
  void centerToolActionSlot();
  void optionChangedSlot();
  void showOrientationSlot(bool visible);
  void globalConfigurationFileChangedSlot(QString key);
  void setStereoEyeAngle(double angle);
  void settingsChangedSlot(QString key);

private:
  virtual void appendToContextMenu(QMenu& contextMenu);
  void updateView();
  void readDataRepSettings(ssc::RepPtr rep);

  void showLandmarks(bool on);
  void showPointPickerProbe(bool on);
  void setOrientationAnnotation();

  ssc::RepPtr createDataRep3D(ssc::DataPtr data);
  virtual void dataAdded(ssc::DataPtr data);
  virtual void dataRemoved(const QString& uid);

//  virtual void imageAdded(ssc::ImagePtr image);
//  virtual void meshAdded(ssc::MeshPtr mesh);
//  virtual void imageRemoved(const QString& uid);
//  virtual void meshRemoved(const QString& uid);

  typedef  std::map<QString, ssc::RepPtr> RepMap;
//  typedef  std::map<QString, ssc::VolumetricRepPtr> VolumetricRepMap;
//  typedef  std::map<QString, ssc::GeometricRepPtr> GeometricRepMap;
  RepMap mDataReps;
//  VolumetricRepMap mVolumetricReps;
  ImageLandmarkRepPtr mImageLandmarkRep;
  PatientLandmarkRepPtr mPatientLandmarkRep;
  ssc::ProbeRepPtr mProbeRep;
//  GeometricRepMap mGeometricReps;
  ssc::DisplayTextRepPtr mPlaneTypeText;
  ssc::DisplayTextRepPtr mDataNameText;
  std::map<QString, ToolAxisConnectorPtr> mToolAxis;
  ssc::AxesRepPtr mRefSpaceAxisRep;
  std::map<QString, ssc::AxesRepPtr> mDataSpaceAxisRep;
//  vtkSmartPointer<class InteractionCallback> mInteractorCallback;

  bool mShowAxes; ///< show 3D axes reps for all tools and ref space

  ssc::SlicePlanes3DRepPtr mSlicePlanes3DRep;
  ssc::OrientationAnnotation3DRepPtr mAnnotationMarker;

  QPointer<ssc::View> mView;
};
typedef boost::shared_ptr<ViewWrapper3D> ViewWrapper3DPtr;

} // namespace cx

#endif /* CXVIEWWRAPPER3D_H_ */
