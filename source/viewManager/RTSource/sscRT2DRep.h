/*
 * sscRT2DRep.h
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */

#ifndef SSCRT2DREP_H_
#define SSCRT2DREP_H_

#include "sscRepImpl.h"
#include "sscRealTimeStreamSource.h"
#include "sscVtkHelperClasses.h"
#include "sscForwardDeclarations.h"
#include "sscProbeData.h"

typedef vtkSmartPointer<class vtkPlaneSource> vtkPlaneSourcePtr;
typedef vtkSmartPointer<class vtkTexture> vtkTexturePtr;
typedef vtkSmartPointer<class vtkTransformTextureCoords> vtkTransformTextureCoordsPtr;
typedef vtkSmartPointer<class vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<class vtkActor> vtkActorPtr;
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkTextureMapToPlane> vtkTextureMapToPlanePtr;
typedef vtkSmartPointer<class vtkImageMask> vtkImageMaskPtr;

class UltrasoundSectorSource;

namespace ssc
{

typedef boost::shared_ptr<class RealTimeStreamGraphics> RealTimeStreamGraphicsPtr;

class RealTimeStreamGraphics : public QObject
{
  Q_OBJECT
public:
  RealTimeStreamGraphics(bool useMaskFilter=false);
  virtual ~RealTimeStreamGraphics();

  void setRealtimeStream(RealTimeStreamSourcePtr data);
  void setTool(ToolPtr tool);
  void setIgnoreToolTransform(bool on);

  vtkActorPtr getActor();

signals:
  void newData();

private slots:
  void newDataSlot();
  void receiveTransforms(Transform3D matrix, double timestamp);
  void receiveVisible(bool visible);
  void probeSectorChanged();

private:
  void initializeSize(int imageWidth, int imageHeight);
  void setLookupTable();

  bool mUseMask;

  bool mIgnoreToolTransform;
  ToolPtr mTool;
  ssc::ProbeData mProbeData;
  RealTimeStreamSourcePtr mData;
  vtkActorPtr mPlaneActor;
  vtkPlaneSourcePtr mPlaneSource;
  vtkTexturePtr mTexture;
//  vtkRendererPtr mRenderer;
//  View* mView;
  UltrasoundSectorSource* mUSSource;
  vtkDataSetMapperPtr mDataSetMapper;
  vtkTransformTextureCoordsPtr mTransformTextureCoords;

  vtkImageThresholdPtr mMapZeroToOne;
//  vtkImageDataPtr mUSMaskData;
  vtkImageMaskPtr mMaskFilter;
};


typedef boost::shared_ptr<class RealTimeStreamRep> RealTimeStreamRepPtr;

class RealTimeStreamRep : public ssc::RepImpl
{
  Q_OBJECT
public:
  RealTimeStreamRep(const QString& uid, const QString& name="");
  virtual ~RealTimeStreamRep();
  virtual QString getType() const { return "ssc::RealTimeStreamRep"; }
  void setRealtimeStream(RealTimeStreamSourcePtr data);
  void setTool(ToolPtr tool);

protected:
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);
private slots:

private:
  RealTimeStreamGraphicsPtr mRTGraphics;
  vtkRendererPtr mRenderer;
  View* mView;
};

typedef boost::shared_ptr<class RealTimeStreamFixedPlaneRep> RealTimeStreamFixedPlaneRepPtr;

/**A rep visualizing a RT stream directly into the view plane.
 * It does not follow the tool, but controls the camera in order to
 * fill the entire view.
 */
class RealTimeStreamFixedPlaneRep : public ssc::RepImpl
{
  Q_OBJECT
public:
  RealTimeStreamFixedPlaneRep(const QString& uid, const QString& name="");
  virtual ~RealTimeStreamFixedPlaneRep();
  virtual QString getType() const { return "ssc::RealTimeStreamFixedPlaneRep"; }
  void setRealtimeStream(RealTimeStreamSourcePtr data);
  void setTool(ToolPtr tool);
//  void setLockCameraToStream(bool on);

protected:
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);
private slots:
  void newDataSlot();

//  void receiveTransforms(Transform3D matrix, double timestamp);
//  void receiveVisible(bool visible);
//  void probeSectorChanged();

private:
//  void initializeSize(int imageWidth, int imageHeight);
//  void setLookupTable();
  void setCamera();
//  void setup();

  RealTimeStreamGraphicsPtr mRTGraphics;

  ToolPtr mTool;
  ssc::ProbeData mProbeData;
  RealTimeStreamSourcePtr mData;

  ssc::TextDisplayPtr mStatusText;
  ssc::TextDisplayPtr mInfoText;

  vtkRendererPtr mRenderer;
  View* mView;
};


} // namespace ssc

#endif /* SSCRT2DREP_H_ */
