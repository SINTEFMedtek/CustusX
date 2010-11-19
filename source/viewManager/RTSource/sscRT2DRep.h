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

typedef boost::shared_ptr<class RealTimeStream2DRep> RealTimeStream2DRepPtr;

class RealTimeStream2DRep : public ssc::RepImpl
{
  Q_OBJECT
public:
  RealTimeStream2DRep(const QString& uid, const QString& name="");
  virtual ~RealTimeStream2DRep();
  virtual QString getType() const { return "ssc::RealTimeStream2DRep"; }
  void setRealtimeStream(RealTimeStreamSourcePtr data);
  void setTool(ToolPtr tool);
  void setLockCameraToStream(bool on);

protected:
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);
private slots:
  void newDataSlot();

  void receiveTransforms(Transform3D matrix, double timestamp);
  void receiveVisible(bool visible);
  void probeSectorChanged();

private:
//  void setMask();
  void initializeSize(int imageWidth, int imageHeight);
  void setLookupTable();
  void setCamera();
  void setup();

//  vtkPolyDataPtr createTestPolyData();

  ToolPtr mTool;
  ssc::ProbeData mProbeData;
  RealTimeStreamSourcePtr mData;
  ssc::TextDisplayPtr mStatusText;
  ssc::TextDisplayPtr mInfoText;
  vtkActorPtr mPlaneActor;
  vtkPlaneSourcePtr mPlaneSource;
  vtkTexturePtr mTexture;
  vtkRendererPtr mRenderer;
  View* mView;
  UltrasoundSectorSource* mUSSource;
  vtkDataSetMapperPtr mDataSetMapper;


//  vtkPolyDataPtr mTestPoly;

  vtkImageThresholdPtr mMapZeroToOne;
  vtkImageDataPtr mUSMaskData;
  vtkImageMaskPtr mMaskFilter;
  bool mOverrideCamera;
};


} // namespace ssc

#endif /* SSCRT2DREP_H_ */
