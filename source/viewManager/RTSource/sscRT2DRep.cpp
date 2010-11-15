/*
 * sscRT2DRep.cpp
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */
#include "sscRT2DRep.h"

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

#include "sscBoundingBox3D.h"
#include "sscToolManager.h"
#include "sscView.h"
#include "sscTool.h"
#include "sscTypeConversions.h"


namespace ssc
{

RealTimeStream2DRep::RealTimeStream2DRep(const QString& uid, const QString& name) :
  ssc::RepImpl(uid, name),
  mPlaneActor(vtkActorPtr::New()),
  mPlaneSource(vtkPlaneSourcePtr::New()),
  mTexture(vtkTexturePtr::New() )
{
  this->setLookupTable();
  mOverrideCamera = false;

  // set a filter that map all zeros in the input to ones. This enables us to
  // use zero as a special transparency value, to be used in masking.
  mMapZeroToOne = vtkImageThresholdPtr::New();
  mMapZeroToOne->ThresholdByLower(1.0);
  mMapZeroToOne->SetInValue(1);
  mMapZeroToOne->SetReplaceIn(true);

  mUSMaskData = mProbeData.getMask();
  mUSMaskData->Print(std::cout);

  // set the filter that applies a mask to the stream data
  mMaskFilter = vtkImageMaskPtr::New();
  mMaskFilter->SetMaskInput(mUSMaskData);
  mMaskFilter->SetMaskedOutputValue(0.0);

  vtkTextureMapToPlanePtr tMapper = vtkTextureMapToPlanePtr::New();
  tMapper->SetInput(mPlaneSource->GetOutput());

  vtkTransformTextureCoordsPtr transform = vtkTransformTextureCoordsPtr::New();
  transform->SetInput(tMapper->GetOutput() );
  transform->SetOrigin( 0, 0, 0);
  transform->SetScale( 1, 1, 0);
  transform->FlipROn();

  vtkDataSetMapperPtr mapper2 = vtkDataSetMapperPtr::New();
  mapper2->SetInput(transform->GetOutput() );
  mapper2->Update();

  mPlaneActor->SetTexture(mTexture);
  mPlaneActor->SetMapper(mapper2);

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

RealTimeStream2DRep::~RealTimeStream2DRep()
{
}

void RealTimeStream2DRep::setTool(ToolPtr tool)
{
  if (tool==mTool)
    return;

  if (mTool)
  {
    disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
    disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
  }

  mTool = tool;

  // setup new
  if (mTool)
  {
    connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
    connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));

    receiveTransforms(mTool->get_prMt(), 0);
//    mToolActor->SetVisibility(mTool->getVisible());
  }
}

/** Create a lut that sets zeros to transparent and applies a linear grayscale to the rest.
 *
 */
void RealTimeStream2DRep::setLookupTable()
{
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
  lut->SetTableValue(0, 0, 0, 0, 0); // set the lowest value to transparent. This will make the masked values transparent, but nothing else

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

void RealTimeStream2DRep::setRealtimeStream(RealTimeStreamSourcePtr data)
{
  std::cout << "RealTimeStream2DRep::setRealtimeStream()" << std::endl;

  if (mData)
  {
    disconnect(mData.get(), SIGNAL(changed()), this, SLOT(newDataSlot()));
    mTexture->SetInput(NULL);
  }

  mData = data;
  bool useMask = true;

  if (mData)
  {
    connect(mData.get(), SIGNAL(changed()), this, SLOT(newDataSlot()));
    if (!useMask) // send data directly to texture, no mask.
    {
      mTexture->SetInput(mData->getVtkImageData());
    }
    else    // these lines convert zeros to ones, then applies the mask.
    {
      mMapZeroToOne->SetInput(mData->getVtkImageData());
      mMaskFilter->SetImageInput(mMapZeroToOne->GetOutput());
      //mMaskFilter->SetImageInput(mData->getVtkImageData());
      mTexture->SetInput(mMaskFilter->GetOutput());
    }

//    std::cout << "mData->getVtkImageData() " << mData->getVtkImageData() << std::endl;
//    std::cout << "mMapZeroToOne->GetOutput() " << mMapZeroToOne->GetOutput() << std::endl;
  }

  this->newDataSlot();
}



void RealTimeStream2DRep::receiveTransforms(Transform3D prMt, double timestamp)
{
  //mProbeData.test();
  Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
  Transform3D tMu = mProbeData.get_tMu();
  //Transform3D rMt = rMpr * prMt;
  Transform3D rMu = rMpr * prMt * tMu;
//  mPlaneActor->SetUserMatrix(rMt.matrix());
  mPlaneActor->SetUserMatrix(rMu.matrix());
}

void RealTimeStream2DRep::receiveVisible(bool visible)
{

}

void RealTimeStream2DRep::newDataSlot()
{
//  std::cout << "p1" << ssc::Vector3D(mPlaneSource->GetPoint1()) << std::endl;
//  std::cout << "p2" << ssc::Vector3D(mPlaneSource->GetPoint2()) << std::endl;
//  std::cout << "RealTimeStream2DRep::newDataSlot()" << std::endl;
  mPlaneActor->SetVisibility(mData!=NULL);
  if (!mData)
    return;
  this->initializeSize(mData->getVtkImageData()->GetDimensions()[0], mData->getVtkImageData()->GetDimensions()[1]);
//    mPlaneActor->SetVisibility(true);
//    setCamera();

  mPlaneActor->SetVisibility(mData->validData());
  mInfoText->updateText(mData->getInfoString());
  mStatusText->updateText(mData->getStatusString());
  mStatusText->getActor()->SetVisibility(!mData->validData());
  //std::cout << "vis: " << mPlaneActor->GetVisibility() << std::endl;

//  vtkImageDataPtr tex = mMaskFilter->GetOutput();
//  tex->Update();
//  std::cout << "dim " << Vector3D(tex->GetDimensions()) << ", sp " << Vector3D(tex->GetSpacing()) << std::endl;

  if (mOverrideCamera)
    this->setCamera();
}

void RealTimeStream2DRep::initializeSize(int imageWidth, int imageHeight)
{
//  std::cout << "RealTimeStream2DRep::initializeSize start" << std::endl;
//  mPlaneSource->GetOutput()->GetPointData()->Print(std::cout);
  //std::cout << "RealTimeStream2DRep::initializeSize("+string_cast(imageWidth)+","+string_cast(imageHeight)+")"  << std::endl;;
  if (imageWidth==0 || imageHeight==0)
  {
    return;
  }
  //std::cout << "data: " << mData->getVtkImageData()->GetScalarTypeAsString() << ", " << mData->getVtkImageData()->GetNumberOfScalarComponents() << std::endl;
  DoubleBoundingBox3D extent(mData->getVtkImageData()->GetExtent());
  if (ssc::similar(extent.range()[0], 0.0) || ssc::similar(extent.range()[1], 0.0))
    return;

  mPlaneSource->SetOrigin(extent.corner(0,0,0).begin());
  mPlaneSource->SetPoint1(extent.corner(1,0,0).begin());
  mPlaneSource->SetPoint2(extent.corner(0,1,0).begin());
//  std::cout << "extent " << extent << std::endl;
//  mPlaneSource->Print(std::cout);

//  int numPts = 3;
//  vtkFloatArray* newTCoords = vtkFloatArray::New();
//  newTCoords->SetNumberOfComponents(2);
//  newTCoords->Allocate(2*numPts);
//
//  float tc[2];
//  tc[0] = 0;
//  tc[1] = 0;
//  newTCoords->InsertTuple(0,tc);
//  tc[0] = 1;
//  tc[1] = 1;
//  newTCoords->InsertTuple(1,tc);
//  tc[0] = 0;
//  tc[1] = 1;
//  newTCoords->InsertTuple(2,tc);
////  tc[0] = 0;
////  tc[1] = 0;
////  newTCoords->InsertTuple(3,tc);
////  tc[0] = 0;
////  tc[1] = 0;
////  newTCoords->InsertTuple(4,tc);
//  newTCoords->SetName("TextureCoordinates");
//
//  mPlaneSource->GetOutput()->GetPointData()->SetTCoords(newTCoords);
//  mPlaneSource->GetOutput()->GetPointData()->Modified();
//  mPlaneSource->GetOutput()->Modified();

  //std::cout << "RealTimeStream2DRep::initializeSize end" << std::endl;
  //mPlaneSource->GetOutput()->GetPointData()->Print(std::cout);
}


/**We need this here, even if it belongs in singlelayout.
 * Reason: must call setcamera after last change of size of plane actor.
 * TODO fix it.
 */
void RealTimeStream2DRep::setCamera()
{
  if (!mRenderer)
    return;
  vtkCamera* camera = mRenderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  mRenderer->ResetCamera();

  DoubleBoundingBox3D extent(mData->getVtkImageData()->GetExtent());
  if (ssc::similar(extent.range()[0], 0.0) || ssc::similar(extent.range()[1], 0.0))
    return;

  camera->SetParallelScale(extent.range()[1]/2); // exactly fill the viewport height
}


void RealTimeStream2DRep::addRepActorsToViewRenderer(ssc::View* view)
{
  mView = view;
  mRenderer = view->getRenderer();

  view->getRenderer()->AddActor(mPlaneActor);
  view->getRenderer()->AddActor(mInfoText->getActor());
  view->getRenderer()->AddActor(mStatusText->getActor());
  //setCamera();
}

void RealTimeStream2DRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mRenderer = vtkRendererPtr();
  view->getRenderer()->RemoveActor(mPlaneActor);
  view->getRenderer()->RemoveActor(mInfoText->getActor());
  view->getRenderer()->RemoveActor(mStatusText->getActor());
}


} // namespace ssc
