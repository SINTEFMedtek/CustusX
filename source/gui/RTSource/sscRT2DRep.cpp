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
#include <vtkLookupTable.h>
#include <vtkImageThreshold.h>
#include "sscBoundingBox3D.h"
#include "sscView.h"

namespace ssc
{


vtkImageDataPtr generateVtkImageData(Vector3D dim, Vector3D spacing, const unsigned char initValue)
{
  vtkImageDataPtr data = vtkImageDataPtr::New();
  data->SetSpacing(spacing[0], spacing[1], spacing[2]);
  data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
  data->SetScalarTypeToUnsignedChar();
  data->SetNumberOfScalarComponents(1);

  int scalarSize = dim[0]*dim[1]*dim[2];

  unsigned char *rawchars = (unsigned char*)malloc(scalarSize+1);
  std::fill(rawchars,rawchars+scalarSize, initValue);

  vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
  array->SetNumberOfComponents(1);
  //TODO: Whithout the +1 the volume is black
  array->SetArray(rawchars, scalarSize+1, 0); // take ownership
  data->GetPointData()->SetScalars(array);

  // A trick to get a full LUT in ssc::Image (automatic LUT generation)
  // Can't seem to fix this by calling Image::resetTransferFunctions() after volume is modified
  rawchars[0] = 255;
  data->GetScalarRange();// Update internal data in vtkImageData. Seems like it is not possible to update this data after the volume has been changed.
  rawchars[0] = 0;

  return data;
}

RealTimeStream2DRep::RealTimeStream2DRep(const QString& uid, const QString& name) :
  ssc::RepImpl(uid, name),
  mPlaneActor(vtkActorPtr::New()),
  mPlaneSource(vtkPlaneSourcePtr::New()),
  mTexture(vtkTexturePtr::New() )
{
  //make a default system set lookuptable, grayscale...
  vtkLookupTablePtr lut = vtkLookupTablePtr::New();
  lut->SetTableRange (0, 1);
  lut->SetSaturationRange (0, 1);
  lut->SetHueRange (0, 0.5);
  lut->SetValueRange (0, 1);
  lut->Build();
  lut->SetTableValue(0, 0, 0, 0, 0);
//  std::cout << "lut # " << lut->GetNumberOfTableValues() << std::endl;
//  for (int i=0; i< lut->GetNumberOfTableValues(); ++i)
//    lut->SetTableValue(i, double(i)/lut->GetNumberOfTableValues(), 0, 0, 1);
  lut->SetTableValue(0, 0, 0, 0, 0);
  //this->setBaseLookupTable(lut);
  mTexture->SetLookupTable(lut);
  mTexture->MapColorScalarsThroughLookupTableOn();
  lut->Modified();

  mMapZeroToOne = vtkImageThresholdPtr::New();
  mMapZeroToOne->ThresholdByLower(0.0);
  mMapZeroToOne->SetInValue(0);
  mMapZeroToOne->SetReplaceIn(true);

//  mAllOneData = generateVtkImageData(Vector3D(256,256,1), Vector3D(1,1,1), 1);
//  mMaskZeroToOneFilter = vtkImageMaskPtr::New();
//  mMaskZeroToOneFilter->SetMaskInput(mAllOneData);
//  mMaskZeroToOneFilter->SetMaskedOutputValue(0.1);


  mUSMaskData = generateVtkImageData(Vector3D(256,256,1), Vector3D(1,1,1), 0);

  int* dim(mUSMaskData->GetDimensions());
  unsigned char* dataPtr = static_cast<unsigned char*>(mUSMaskData->GetScalarPointer());
  for(int x = 0; x < dim[0]; x++)
    for(int y = 0; y < dim[1]; y++)
    {
      bool inside = (Vector3D(x,y,0)-Vector3D(100,100,0)).length() < 100;


      if(inside)
        dataPtr[x + y*dim[0]] = 1;
      else
        dataPtr[x + y*dim[0]] = 0;
    }


  mMaskFilter = vtkImageMaskPtr::New();
  mMaskFilter->SetMaskInput(mUSMaskData);
  mMaskFilter->SetMaskedOutputValue(0.0);

//  mData = data;
//  //connect(mData.get(), SIGNAL(statusChanged()), this, SLOT(statusChangedSlot()));
//  connect(mData.get(), SIGNAL(newData()), this, SLOT(newDataSlot()));

//  mTexture->SetInput(mData->getVtkImageData());

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
//  statusChangedSlot();
  //Logger::log("vm.log", "RealTimeStream2DRep::RealTimeStream2DRep()");
}

RealTimeStream2DRep::~RealTimeStream2DRep()
{
//  Logger::log("vm.log", "RealTimeStream2DRep::~RealTimeStream2DRep()");
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

  if (mData)
  {
    connect(mData.get(), SIGNAL(changed()), this, SLOT(newDataSlot()));
//    mTexture->SetInput(mData->getVtkImageData());
    mMapZeroToOne->SetInput(mData->getVtkImageData());
    mMaskFilter->SetImageInput(mMapZeroToOne->GetOutput());
    //mMaskFilter->SetImageInput(mData->getVtkImageData());
    mTexture->SetInput(mMaskFilter->GetOutput());

    std::cout << "mData->getVtkImageData() " << mData->getVtkImageData() << std::endl;
    std::cout << "mMapZeroToOne->GetOutput() " << mMapZeroToOne->GetOutput() << std::endl;
    mMapZeroToOne->Update();
  }

  this->newDataSlot();
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

  this->setCamera();
}

void RealTimeStream2DRep::initializeSize(int imageWidth, int imageHeight)
{
//  Logger::log("vm.log", "RealTimeStream2DRep::initializeSize("+string_cast(imageWidth)+","+string_cast(imageHeight)+")");
  if (imageWidth==0 || imageHeight==0)
  {
    return;
  }
  DoubleBoundingBox3D extent(mData->getVtkImageData()->GetExtent());
  if (ssc::similar(extent.range()[0], 0.0) || ssc::similar(extent.range()[1], 0.0))
    return;

  //mPlaneSource->SetPoint2( 0.0, imageHeight - 1, 0.0 );
  //mPlaneSource->SetPoint1(imageWidth - 1, 0.0, 0.0 );
  mPlaneSource->SetOrigin(extent.corner(0,0,0).begin());
  mPlaneSource->SetPoint1(extent.corner(1,0,0).begin());
  mPlaneSource->SetPoint2(extent.corner(0,1,0).begin());
//  std::cout << "extent " << extent << std::endl;
//  mPlaneSource->Print(std::cout);
}

//void RealTimeStream2DRep::statusChangedSlot()
//{
////  initializeSize(mData->width(), mData->height());
////  mPlaneActor->SetVisibility(mData->valid());
////  formatMechTermIndex();
////  mStatusText->updateText(mData->statusString() );
////  setCamera();
////
////  Logger::log("vm.log", "US statusChanged: " + string_cast(mData->width()) + ", " + string_cast(mData->height())
////      + ", connected: " + string_cast(mData->connected()) );
//
//}

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

//void RealTimeStream2DRep::setup()
//{
//  mInfoText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 16));
//  mInfoText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
//  mInfoText->setCentered();
//  mInfoText->setPosition(0.5, 0.05);
//
//  mStatusText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 20));
//  mStatusText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
//  mStatusText->setCentered();
//  mStatusText->setPosition(0.5, 0.5);
//  mStatusText->updateText("testimage");
//
//  //setCamera();
//}

//void RealTimeStream2DRep::formatMechTermIndex()
//{
//  char buffer[100];
//  snprintf(buffer, sizeof(buffer), "MI %3.1f TI %3.1f", mData->mechanicalIndex(), mData->thermalIndex() );
//  mInfoText->updateText(std::string(buffer));
//}


} // namespace ssc
