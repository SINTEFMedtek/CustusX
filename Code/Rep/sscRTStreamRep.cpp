/*
 * sscRT2DRep.cpp
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */
#include "sscRTStreamRep.h"

#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkTransformTextureCoords.h>
#include <vtkTextureMapToPlane.h>
#include <vtkDataSetMapper.h>
#include <vtkTexture.h>
#include <vtkCamera.h>
#include "vtkImageMask.h"
#include <vtkPointData.h>
#include <vtkMatrix4x4.h>
#include <vtkLookupTable.h>
#include <vtkImageThreshold.h>
#include <vtkFloatArray.h>
#include <vtkTextureMapToPlane.h>
#include <vtkCellArray.h>
#include <vtkImageChangeInformation.h>
#include "sscBoundingBox3D.h"
#include "sscToolManager.h"
#include "sscView.h"
#include "sscTool.h"
#include "sscTypeConversions.h"
#include  "ultrasoundsectorsource.h"

namespace ssc
{


RealTimeStreamGraphics::RealTimeStreamGraphics(bool useMaskFilter) :
  mPlaneActor(vtkActorPtr::New()),
  mPlaneSource(vtkPlaneSourcePtr::New()),
  mTexture(vtkTexturePtr::New() )
{
  mDataRedirecter = vtkImageChangeInformationPtr::New();
  mUseMask = useMaskFilter;
  mIgnoreToolTransform = false;
  mUSSource = UltrasoundSectorSource::New();
  mUSSource->setProbeSector(mProbeData.getSector());

  this->setLookupTable();

  if (mUseMask)
  {
    // set a filter that map all zeros in the input to ones. This enables us to
    // use zero as a special transparency value, to be used in masking.
    mMapZeroToOne = vtkImageThresholdPtr::New();
    mMapZeroToOne->ThresholdByLower(1.0);
    mMapZeroToOne->SetInValue(1);
    mMapZeroToOne->SetReplaceIn(true);

    // set the filter that applies a mask to the stream data
    mMaskFilter = vtkImageMaskPtr::New();
    mMaskFilter->SetMaskInput(mProbeData.getMask());
    mMaskFilter->SetMaskedOutputValue(0.0);
  }

  // generate texture coords for mPlaneSource
  vtkTextureMapToPlanePtr tMapper = vtkTextureMapToPlanePtr::New();
  tMapper->SetInput(mPlaneSource->GetOutput());

  mTransformTextureCoords = vtkTransformTextureCoordsPtr::New();
  mTransformTextureCoords->SetInput(tMapper->GetOutput() );
  mTransformTextureCoords->SetOrigin( 0, 0, 0);
  mTransformTextureCoords->SetScale( 1, 1, 0);
  mTransformTextureCoords->FlipROn();

  // all paths to go into the DataSetMapper
  mDataSetMapper = vtkDataSetMapperPtr::New();
  mDataSetMapper->SetInput(mTransformTextureCoords->GetOutput() );
//  mapper2->SetInput(mUSSource->GetOutput() );
  mDataSetMapper->Update();

  mPlaneActor->SetTexture(mTexture);
  mPlaneActor->SetMapper(mDataSetMapper);
}

RealTimeStreamGraphics::~RealTimeStreamGraphics()
{
}

void RealTimeStreamGraphics::setIgnoreToolTransform(bool on)
{
  mIgnoreToolTransform = on;
}

vtkActorPtr RealTimeStreamGraphics::getActor()
{
  return mPlaneActor;
}

void RealTimeStreamGraphics::setTool(ToolPtr tool)
{
  if (tool==mTool)
    return;

  if (mTool)
  {
    disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
    disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
    disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));
  }

  mTool = tool;

  // setup new
  if (mTool)
  {
    connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
    connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
    connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));

//    receiveTransforms(mTool->get_prMt(), 0);
//    mProbeData.setSector(mTool->getProbeSector());
//    mUSSource->setProbeSector(mProbeData.getSector());
//    std::cout << "setting tool in rt rep" << std::endl;
//    mToolActor->SetVisibility(mTool->getVisible());

    if (mUseMask)
    {
      // do nothing: keep the pipeline from PlaneSource
    }
    else
    {
      // now that we have a tool: use the ultraound source, updated by the probe
      mTransformTextureCoords->SetInput(mUSSource->GetOutput() );
    }

    this->probeSectorChanged();
  }
}

void RealTimeStreamGraphics::probeSectorChanged()
{
  if (!mTool)
    return;

  mProbeData.setSector(mTool->getProbeSector());
  mDataRedirecter->SetOutputSpacing(mTool->getProbeSector().mImage.mSpacing.begin());

  if (mUseMask)
    mMaskFilter->SetMaskInput(mProbeData.getMask());
  else
    mUSSource->setProbeSector(mProbeData.getSector());

  receiveTransforms(mTool->get_prMt(), 0);
}

/** Create a lut that sets zeros to transparent and applies a linear grayscale to the rest.
 *
 */
void RealTimeStreamGraphics::setLookupTable()
{
  // applies only to mask:
  // Create a lut of size at least equal to the data range. Set the tableRange[0] to zero.
  // This will force input zero to be mapped onto the first table value (the transparent one),
  // and inputs [1, -> > is mapped to larger values, not transparent.
  // In order to create a window-level function, manually build a table.

  int N = 1400;
  //make a default system set lookuptable, grayscale...
  vtkLookupTablePtr lut = vtkLookupTablePtr::New();
  lut->SetNumberOfTableValues(N);
  //lut->SetTableRange (0, 1024); // the window of the input
  lut->SetTableRange (0, N-1); // the window of the input
  lut->SetSaturationRange (0, 0);
  lut->SetHueRange (0, 0);
  lut->SetValueRange (0, 1);
  lut->Build();
  if (mUseMask)
  {
    lut->SetTableValue(0, 0, 0, 0, 0); // set the lowest value to transparent. This will make the masked values transparent, but nothing else
  }

//  lut->SetNumberOfTableValues(3);
//  lut->SetTableRange(0, pow(2, 16)-1);
//  lut->SetTableValue(0, 0, 0, 0, 0);
//  lut->SetTableValue(1, 0, 0, 0, 1);
//  lut->SetTableValue(1400, 1, 1, 1, 1);

//  std::cout << "lut # " << lut->GetNumberOfTableValues() << std::endl;
//  double N = lut->GetNumberOfTableValues();
//  for (int i=0; i<N; ++i)
//    lut->SetTableValue(i, 0, 0, double(i)/N, 1);
//  lut->SetTableValue(0, 0, 0, 0, 0);

// disable these two lines to remove lut
  mTexture->SetLookupTable(lut);
  mTexture->MapColorScalarsThroughLookupTableOn();
  lut->Modified();


}

void RealTimeStreamGraphics::setRealtimeStream(RealTimeStreamSourcePtr data)
{
  if (mData)
  {
    disconnect(mData.get(), SIGNAL(changed()), this, SLOT(newDataSlot()));
    mTexture->SetInput(NULL);
  }

  mData = data;

  if (mData)
  {
    connect(mData.get(), SIGNAL(changed()), this, SLOT(newDataSlot()));

    mDataRedirecter->SetInput(mData->getVtkImageData());
//    mDataRedirecter->SetOutputSpacing(mTool->getProbeSector().mImage.mSpacing.begin());
//    mDataRedirecter->GetOutput();


    if (!mUseMask) // send data directly to texture, no mask.
    {
      mTexture->SetInput(mDataRedirecter->GetOutput());
    }
    else    // these lines convert zeros to ones, then applies the mask.
    {
      mMapZeroToOne->SetInput(mDataRedirecter->GetOutput());
      mMaskFilter->SetImageInput(mMapZeroToOne->GetOutput());
      mTexture->SetInput(mMaskFilter->GetOutput());
    }
  }

  this->newDataSlot();
}

void RealTimeStreamGraphics::receiveTransforms(Transform3D prMt, double timestamp)
{
  if (mIgnoreToolTransform)
    return;
  //mProbeData.test();
  Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
  Transform3D tMu = mProbeData.get_tMu();
  //Transform3D rMt = rMpr * prMt;
  Transform3D rMt = rMpr * prMt;
  Transform3D rMu = rMpr * prMt * tMu;
//  mPlaneActor->SetUserMatrix(rMt.matrix());
  mPlaneActor->SetUserMatrix(rMu.matrix());
//  mPlaneActor->SetUserMatrix(rMt.matrix());
}

void RealTimeStreamGraphics::receiveVisible(bool visible)
{

}


void RealTimeStreamGraphics::checkDataIntegrity()
{
  if (!mData || !mTool)
    return;

  std::cout << "probe sector " << mTool->getUid() << " " << streamXml2String(mTool->getProbeSector()) << std::endl;
  DoubleBoundingBox3D bounds_poly_u(mProbeData.getSector()->GetBounds());
  DoubleBoundingBox3D bounds_poly_v = transform(mProbeData.get_uMv().inv(), bounds_poly_u);
  DoubleBoundingBox3D bounds(mDataRedirecter->GetOutput()->GetBounds());
  DoubleBoundingBox3D extent(mDataRedirecter->GetOutput()->GetExtent());
  std::cout << "poly_u bounds: " << bounds_poly_u << std::endl;
  std::cout << "poly_v bounds: " << bounds_poly_v << std::endl;
  std::cout << "img bounds: " << bounds << std::endl;
  std::cout << "img extent: " << extent << std::endl;
  std::cout << "img spacing: " << Vector3D(mDataRedirecter->GetOutput()->GetSpacing()) << std::endl;

//  mDataRedirecter->GetOutput()->Print(std::cout);
}

void RealTimeStreamGraphics::newDataSlot()
{
//  std::cout << "RealTimeStreamGraphics::newDataSlot()" << std::endl;
  mPlaneActor->SetVisibility(mData!=NULL);
  if (!mData)
    return;

//  mDataRedirecter->SetInput(mData->getVtkImageData());
//  mDataRedirecter->SetOutputSpacing(mTool->getProbeSector().mImage.mSpacing.begin());
//  mDataRedirecter->GetOutput();

  mDataRedirecter->GetOutput()->Update();
//  this->checkDataIntegrity();

  // apply a lut only if the input data is monochrome
  int numComp = mDataRedirecter->GetOutput()->GetNumberOfScalarComponents();
  bool is8bit = mDataRedirecter->GetOutput()->GetScalarType()==VTK_UNSIGNED_CHAR;
  if (numComp==1 && !is8bit)
    mTexture->MapColorScalarsThroughLookupTableOn();
  else
    mTexture->MapColorScalarsThroughLookupTableOff();

  // set the planesource where we have no probedata.
  DoubleBoundingBox3D bounds(mDataRedirecter->GetOutput()->GetBounds());
  if (!ssc::similar(bounds.range()[0], 0.0) || !ssc::similar(bounds.range()[1], 0.0))
  {
    mPlaneSource->SetOrigin(bounds.corner(0,0,0).begin());
    mPlaneSource->SetPoint1(bounds.corner(1,0,0).begin());
    mPlaneSource->SetPoint2(bounds.corner(0,1,0).begin());
    mPlaneSource->GetOutput()->GetPointData()->Modified();
    mPlaneSource->GetOutput()->Modified();
  }

  mPlaneActor->SetVisibility(mData->validData());

  emit newData();
}

//---------------------------------------------------------
//---------------------------------------------------------

///////////////////////////////////////////////////////////

//---------------------------------------------------------
//---------------------------------------------------------


RealTimeStreamRep::RealTimeStreamRep(const QString& uid, const QString& name) :
  ssc::RepImpl(uid, name)
{
    mRTGraphics.reset(new RealTimeStreamGraphics());
}

RealTimeStreamRep::~RealTimeStreamRep()
{
}

void RealTimeStreamRep::setTool(ToolPtr tool)
{
  mRTGraphics->setTool(tool);
}

void RealTimeStreamRep::setRealtimeStream(RealTimeStreamSourcePtr data)
{
  mRTGraphics->setRealtimeStream(data);
}

void RealTimeStreamRep::addRepActorsToViewRenderer(ssc::View* view)
{
  mView = view;
  mRenderer = view->getRenderer();

  view->getRenderer()->AddActor(mRTGraphics->getActor());
}

void RealTimeStreamRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mRenderer = vtkRendererPtr();
  view->getRenderer()->RemoveActor(mRTGraphics->getActor());
}



//---------------------------------------------------------
//---------------------------------------------------------

///////////////////////////////////////////////////////////

//---------------------------------------------------------
//---------------------------------------------------------


RealTimeStreamFixedPlaneRep::RealTimeStreamFixedPlaneRep(const QString& uid, const QString& name) :
  ssc::RepImpl(uid, name)
{
  mRTGraphics.reset(new RealTimeStreamGraphics());
  connect(mRTGraphics.get(), SIGNAL(newData()), this, SLOT(newDataSlot()));
  mRTGraphics->setIgnoreToolTransform(true);

  mInfoText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 16));
  mInfoText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  mInfoText->setCentered();
  mInfoText->setPosition(0.5, 0.05);

  mStatusText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 20));
  mStatusText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  mStatusText->setCentered();
  mStatusText->setPosition(0.5, 0.5);
  mStatusText->updateText("testimage");
}

RealTimeStreamFixedPlaneRep::~RealTimeStreamFixedPlaneRep()
{
}

void RealTimeStreamFixedPlaneRep::setTool(ToolPtr tool)
{
  mRTGraphics->setTool(tool);
}

void RealTimeStreamFixedPlaneRep::setRealtimeStream(RealTimeStreamSourcePtr data)
{
  mRTGraphics->setRealtimeStream(data);
  mData = data;
}

void RealTimeStreamFixedPlaneRep::newDataSlot()
{
  if (!mData)
    return;

  mInfoText->updateText(mData->getInfoString());
  mStatusText->updateText(mData->getStatusString());
  mStatusText->getActor()->SetVisibility(!mData->validData());
  this->setCamera();
}

/**We need this here, even if it belongs in singlelayout.
 * Reason: must call setcamera after last change of size of plane actor.
 * TODO fix it.
 */
void RealTimeStreamFixedPlaneRep::setCamera()
{
  if (!mRenderer)
    return;
  vtkCamera* camera = mRenderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  mRenderer->ResetCamera();

//  DoubleBoundingBox3D bounds(mData->getVtkImageData()->GetBounds());
  DoubleBoundingBox3D bounds(mRTGraphics->getActor()->GetBounds());
  if (ssc::similar(bounds.range()[0], 0.0) || ssc::similar(bounds.range()[1], 0.0))
    return;

//  std::cout << "-------------" << std::endl;

  double* vpRange = mRenderer->GetAspect();

  double vw = vpRange[0];
  double vh = vpRange[1];

  double w = bounds.range()[0];
  double h = bounds.range()[1];

  double scale = 1;
  double w_vp = vh * (w/h); // width of image in viewport space
  if (w_vp > vw) // if image too wide: reduce scale
    scale = w_vp/vw;

  camera->SetParallelScale(h/2*scale*1.01); // exactly fill the viewport height
}


void RealTimeStreamFixedPlaneRep::addRepActorsToViewRenderer(ssc::View* view)
{
  mView = view;
  mRenderer = view->getRenderer();

  view->getRenderer()->AddActor(mRTGraphics->getActor());
  view->getRenderer()->AddActor(mInfoText->getActor());
  view->getRenderer()->AddActor(mStatusText->getActor());
  //setCamera();
}

void RealTimeStreamFixedPlaneRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mRenderer = vtkRendererPtr();
  view->getRenderer()->RemoveActor(mRTGraphics->getActor());
  view->getRenderer()->RemoveActor(mInfoText->getActor());
  view->getRenderer()->RemoveActor(mStatusText->getActor());
}

} // namespace ssc
