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

#include "sscBoundingBox3D.h"
#include "sscView.h"

namespace ssc
{




RealTimeStream2DRep::RealTimeStream2DRep(const QString& uid, const QString& name) :
  ssc::RepImpl(uid, name),
  mPlaneActor(vtkActorPtr::New()),
  mPlaneSource(vtkPlaneSourcePtr::New()),
  mUsTexture(vtkTexturePtr::New() )
{
//  mData = data;
//  //connect(mData.get(), SIGNAL(statusChanged()), this, SLOT(statusChangedSlot()));
//  connect(mData.get(), SIGNAL(newData()), this, SLOT(newDataSlot()));

//  mUsTexture->SetInput(mData->getVtkImageData());

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

  mPlaneActor->SetTexture(mUsTexture);
  mPlaneActor->SetMapper(mapper2);

  setup();
//  statusChangedSlot();
  //Logger::log("vm.log", "RealTimeStream2DRep::RealTimeStream2DRep()");
}

RealTimeStream2DRep::~RealTimeStream2DRep()
{
//  Logger::log("vm.log", "RealTimeStream2DRep::~RealTimeStream2DRep()");
}

void RealTimeStream2DRep::setRealtimeStream(RealTimeStreamSourcePtr data)
{
  if (mData)
  {
    disconnect(mData.get(), SIGNAL(newData()), this, SLOT(newDataSlot()));
    mUsTexture->SetInput(NULL);
  }

  mData = data;

  if (mData)
  {
    connect(mData.get(), SIGNAL(newData()), this, SLOT(newDataSlot()));
    mUsTexture->SetInput(mData->getVtkImageData());
  }

  this->newDataSlot();
}

void RealTimeStream2DRep::newDataSlot()
{
  mPlaneActor->SetVisibility(mData!=NULL);
  if (!mData)
    return;
  this->initializeSize(mData->getVtkImageData()->GetDimensions()[0], mData->getVtkImageData()->GetDimensions()[1]);
//    mPlaneActor->SetVisibility(true);
//    setCamera();
}

void RealTimeStream2DRep::initializeSize(int imageWidth, int imageHeight)
{
//  Logger::log("vm.log", "RealTimeStream2DRep::initializeSize("+string_cast(imageWidth)+","+string_cast(imageHeight)+")");
  if (imageWidth==0 || imageHeight==0)
  {
    return;
  }
  DoubleBoundingBox3D extent(mData->getVtkImageData()->GetExtent());
//  mPlaneSource->SetPoint2( 0.0, imageHeight - 1, 0.0 );
//  mPlaneSource->SetPoint1(imageWidth - 1, 0.0, 0.0 );
  mPlaneSource->SetPoint1(0, 0, 0);
  mPlaneSource->SetPoint2(extent[1], extent[3], 0.0);
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

///**We need this here, even if it belongs in singlelayout.
// * Reason: must call setcamera after last change of size of plane actor.
// * TODO fix it.
// */
//void RealTimeStream2DRep::setCamera()
//{
//  if (!mRenderer)
//    return;
//  vtkCamera* camera = mRenderer->GetActiveCamera();
//  camera->ParallelProjectionOn();
//  mRenderer->ResetCamera();
//
//  double scale = camera->GetParallelScale();
//
//  //SW_LOG("%p, %p, %f", mRenderer.GetPointer(), this, scale);
//
//  //SW_LOG("Scale %f ", scale );
//  camera->SetParallelScale(scale*0.6);
//}


void RealTimeStream2DRep::addRepActorsToViewRenderer(ssc::View* view)
{


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

void RealTimeStream2DRep::setup()
{
  mInfoText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 16));
  mInfoText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  mInfoText->setCentered();
  mInfoText->setPosition(0.5, 0.05);

  mStatusText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 20));
  mStatusText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  mStatusText->setCentered();
  mStatusText->setPosition(0.5, 0.5);
  mStatusText->updateText("testimage");

  //setCamera();
}

//void RealTimeStream2DRep::formatMechTermIndex()
//{
//  char buffer[100];
//  snprintf(buffer, sizeof(buffer), "MI %3.1f TI %3.1f", mData->mechanicalIndex(), mData->thermalIndex() );
//  mInfoText->updateText(std::string(buffer));
//}


} // namespace ssc
