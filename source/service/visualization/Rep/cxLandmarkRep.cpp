#include "cxLandmarkRep.h"

#include <sstream>
#include <vtkMath.h>
#include <vtkImageData.h>
#include <vtkDoubleArray.h>
#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "sscView.h"
#include "sscLandmark.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"
#include "boost/bind.hpp"
#include "sscToolManager.h"

namespace cx
{

PatientLandmarksSource::PatientLandmarksSource()
{
  ssc::ToolManager* toolmanager = ssc::ToolManager::getInstance();
  connect(toolmanager, SIGNAL(landmarkAdded(QString)), this, SIGNAL(changed()));
  connect(toolmanager, SIGNAL(landmarkRemoved(QString)), this, SIGNAL(changed()));
  connect(toolmanager, SIGNAL(rMprChanged()), this, SIGNAL(changed()));
}
ssc::LandmarkMap PatientLandmarksSource::getLandmarks() const
{
  return ssc::ToolManager::getInstance()->getLandmarks();
}
ssc::Transform3D PatientLandmarksSource::get_rMl() const
{
  return *ssc::ToolManager::getInstance()->get_rMpr();
}
// --------------------------------------------------------
ssc::Vector3D PatientLandmarksSource::getTextPos(ssc::Vector3D p_l) const
{
  return p_l;
}
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


ImageLandmarksSource::ImageLandmarksSource()
{
}
void ImageLandmarksSource::setImage(ssc::ImagePtr image)
{
  if (image == mImage)
    return;

  if (mImage)
  {
    disconnect(mImage.get(), SIGNAL(landmarkAdded(QString)), this, SIGNAL(changed()));
    disconnect(mImage.get(), SIGNAL(landmarkRemoved(QString)), this, SIGNAL(changed()));
    disconnect(mImage.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
  }

  mImage = image;

  if (mImage)
  {
    connect(mImage.get(), SIGNAL(landmarkAdded(QString)), this, SIGNAL(changed()));
    connect(mImage.get(), SIGNAL(landmarkRemoved(QString)), this, SIGNAL(changed()));
    connect(mImage.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
  }

  emit changed();
}
ssc::LandmarkMap ImageLandmarksSource::getLandmarks() const
{
  if (!mImage)
    return ssc::LandmarkMap();
  return mImage->getLandmarks();
}
ssc::Transform3D ImageLandmarksSource::get_rMl() const
{
  if (!mImage)
    return ssc::Transform3D::Identity();
  return mImage->get_rMd();
}
ssc::Vector3D ImageLandmarksSource::getTextPos(ssc::Vector3D p_l) const
{
  ssc::Vector3D imageCenter = mImage->boundingBox().center();
  ssc::Vector3D centerToSkinVector = (p_l - imageCenter).normal();
  ssc::Vector3D numberPosition = p_l + 10.0 * centerToSkinVector;
  return numberPosition;
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

LandmarkRepPtr LandmarkRep::New(const QString& uid, const QString& name)
{
  LandmarkRepPtr retval(new LandmarkRep(uid, name));
  retval->mSelf = retval;
  return retval;
}

LandmarkRep::LandmarkRep(const QString& uid, const QString& name) :
  RepImpl(uid, name), mColor(0, 1, 0),
  //  mSecondaryColor(0,0.6,0.8),
      mSecondaryColor(0, 0.9, 0.5), mShowLandmarks(true), mGraphicsSize(1), mLabelSize(2.5)
{
  connect(ssc::dataManager(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(internalUpdate()));

  mViewportListener.reset(new ssc::ViewportListener);
  mViewportListener->setCallback(boost::bind(&LandmarkRep::rescale, this));
}

LandmarkRep::~LandmarkRep()
{
}

void LandmarkRep::setPrimarySource(LandmarksSourcePtr primary)
{
//  std::cout << this <<  "  LandmarkRep::setPrimarySource " << primary.get() << std::endl;

  if (mPrimary)
    disconnect(mPrimary.get(), SIGNAL(changed()), this, SLOT(internalUpdate()));
  mPrimary = primary;
  if (mPrimary)
    connect(mPrimary.get(), SIGNAL(changed()), this, SLOT(internalUpdate()));

  this->internalUpdate();
}

void LandmarkRep::setSecondarySource(LandmarksSourcePtr secondary)
{
  if (mSecondary)
    disconnect(mSecondary.get(), SIGNAL(changed()), this, SLOT(internalUpdate()));
  mSecondary = secondary;
  if (mSecondary)
    connect(mSecondary.get(), SIGNAL(changed()), this, SLOT(internalUpdate()));

  this->internalUpdate();
}

void LandmarkRep::setColor(ssc::Vector3D color)
{
  mColor = color;
  this->internalUpdate();
}

void LandmarkRep::setSecondaryColor(ssc::Vector3D color)
{
  mSecondaryColor = color;
  this->internalUpdate();
}

void LandmarkRep::setGraphicsSize(double size)
{
  mGraphicsSize = size;
  this->internalUpdate();
}

void LandmarkRep::setLabelSize(double size)
{
  mLabelSize = size;
  this->internalUpdate();
}

void LandmarkRep::showLandmarks(bool on)
{
  if (on == mShowLandmarks)
    return;

  for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
  {
    if (iter->second.mPrimaryPoint)
      iter->second.mPrimaryPoint->getActor()->SetVisibility(on);
    if (iter->second.mSecondaryPoint)
      iter->second.mSecondaryPoint->getActor()->SetVisibility(on);
    if (iter->second.mText)
      iter->second.mText->getActor()->SetVisibility(on);
    if (iter->second.mLine)
      iter->second.mLine->getActor()->SetVisibility(on);
  }
  mShowLandmarks = on;
}

void LandmarkRep::addAll()
{
//  std::cout << this << " LandmarkRep::addLandmark ADD ALL" << std::endl;

  ssc::LandmarkPropertyMap props = ssc::dataManager()->getLandmarkProperties();

  for (ssc::LandmarkPropertyMap::iterator it = props.begin(); it != props.end(); ++it)
  {
    this->addLandmark(it->first);
  }
}

void LandmarkRep::internalUpdate()
{
//  std::cout << this << " LandmarkRep::internalUpdate()" << std::endl;

  this->clearAll();
  this->addAll();
}

void LandmarkRep::clearAll()
{
//  std::cout << this << " LandmarkRep::addLandmark CLEAR ALL" << std::endl;
  mGraphics.clear();
}

void LandmarkRep::addRepActorsToViewRenderer(ssc::View* view)
{
  if (!view || !view->getRenderer())
    return;

  this->addAll();
  mViewportListener->startListen(view->getRenderer());
}

void LandmarkRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  this->clearAll();
  mViewportListener->stopListen();
}

/**
 * Use the inpup coord in ref space to render a landmark.
 */
void LandmarkRep::addLandmark(QString uid)
{
//  std::cout << this << " LandmarkRep::addLandmark init" << uid << std::endl;
  vtkRendererPtr renderer;
  if (!mViews.empty())
    renderer = (*mViews.begin())->getRenderer();

  ssc::LandmarkProperty property = ssc::dataManager()->getLandmarkProperties()[uid];
  if (property.getUid().isEmpty())
  {
//    std::cout << "LandmarkRep::addLandmark CLEAR" << uid << std::endl;
    mGraphics.erase(uid);
    return;
  }

  LandmarkGraphics current;

  // primary point
  ssc::Landmark primary;
  ssc::Vector3D primary_r;
  if (mPrimary)
  {
//    std::cout << this << "   LandmarkRep::addLandmark found mPrimary" << uid << std::endl;
    primary = mPrimary->getLandmarks()[uid];
    if (!primary.getUid().isEmpty())
    {
//      std::cout << this << "  LandmarkRep::addLandmark" << uid << std::endl;

      primary_r = mPrimary->get_rMl().coord(primary.getCoord());

      current.mPrimaryPoint.reset(new ssc::GraphicalPoint3D(renderer));
      current.mPrimaryPoint->setColor(mColor);
      current.mPrimaryPoint->setRadius(2);

      current.mText.reset(new ssc::FollowerText3D(renderer));
      current.mText->setText(property.getName());
      current.mText->setSizeInNormalizedViewport(true, 0.025);
      current.mText->setColor(mColor);

      ssc::Vector3D text_r = mPrimary->get_rMl().coord(mPrimary->getTextPos(primary.getCoord()));

      current.mPrimaryPoint->setValue(primary_r);
      current.mText->setPosition(text_r);
    }
  }

  // secondary point
  ssc::Vector3D secondary_r;
  ssc::Landmark secondary;
  if (mSecondary)
  {
    secondary = mSecondary->getLandmarks()[uid];
    if (!secondary.getUid().isEmpty())
    {
      secondary_r = mSecondary->get_rMl().coord(secondary.getCoord());

      current.mSecondaryPoint.reset(new ssc::GraphicalPoint3D(renderer));
      current.mSecondaryPoint->setColor(mSecondaryColor);
      current.mSecondaryPoint->setRadius(2);

      current.mSecondaryPoint->setValue(secondary_r);
    }
  }

  // connecting line
  if (!secondary.getUid().isEmpty() && !secondary.getUid().isEmpty())
  {
    current.mLine.reset(new ssc::GraphicalLine3D(renderer));
    current.mLine->setColor(mSecondaryColor);
    current.mLine->setStipple(0x0F0F);

    current.mLine->setValue(primary_r, secondary_r);
  }

  mGraphics[uid] = current;
  this->rescale();
}

void LandmarkRep::rescale()
{
  if (!mViewportListener->isListening())
    return;
  double size = mViewportListener->getVpnZoom();
  double sphereSize = mGraphicsSize / 100 / size;

  for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
  {
    if (iter->second.mSecondaryPoint)
      iter->second.mSecondaryPoint->setRadius(sphereSize);
    if (iter->second.mPrimaryPoint)
      iter->second.mPrimaryPoint->setRadius(sphereSize);
  }
}

}//namespace cx
