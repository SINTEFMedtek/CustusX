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

namespace cx
{

LandmarkRep::LandmarkRep(const QString& uid, const QString& name) :
  RepImpl(uid, name),
  mColor(0,1,0),
  mShowLandmarks(true),
  mShowLabel(true),
  mShowLine(false),
  mGraphicsSize(1),
  mLabelSize(2.5)
{
  mTextScale[0] = mTextScale[1] = mTextScale[2] = 20;
  connect(ssc::dataManager(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(internalUpdate()));

  mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&LandmarkRep::rescale, this));
}

LandmarkRep::~LandmarkRep()
{}

void LandmarkRep::setColor(ssc::Vector3D color)
{
  mColor = color;
}

void LandmarkRep::setGraphicsSize(double size)
{
  mGraphicsSize = size;
  internalUpdate();
}

void LandmarkRep::setLabelSize(double size)
{
  mLabelSize = size;
  internalUpdate();
}

void LandmarkRep::showLandmarks(bool on)
{
  if (on == mShowLandmarks)
    return;

  for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
  {
    iter->second.mPoint->getActor()->SetVisibility(on);
    if (iter->second.mText)
    	iter->second.mText->getActor()->SetVisibility(on);
    if (iter->second.mLine)
      iter->second.mLine->getActor()->SetVisibility(on);
  }
  mShowLandmarks = on;
}

void LandmarkRep::landmarkAddedSlot(QString uid)
{
  this->addPoint(uid);
  this->setPosition(uid);
  this->internalUpdate();
}

void LandmarkRep::landmarkRemovedSlot(QString uid)
{
  mGraphics.erase(uid);
}

void LandmarkRep::transformChangedSlot()
{
	this->addAll();
}

void LandmarkRep::clearAll()
{
  mGraphics.clear();
}


void LandmarkRep::addRepActorsToViewRenderer(ssc::View* view)
{
  if (!view || !view->getRenderer())
    return;

  for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
  {
    iter->second.mPoint->setRenderer(view->getRenderer());
    if (iter->second.mText)
    	iter->second.mText->setRenderer(view->getRenderer());
    if (iter->second.mLine)
      iter->second.mLine->setRenderer(view->getRenderer());
  }
	mViewportListener->startListen(view->getRenderer());
}

void LandmarkRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
  {
    iter->second.mPoint->setRenderer(NULL);
    if (iter->second.mText)
    	iter->second.mText->setRenderer(NULL);
    if (iter->second.mLine)
      iter->second.mLine->setRenderer(NULL);
  }
	mViewportListener->stopListen();
}

/**
 * Use the inpup coord in ref space to render a landmark.
 */
void LandmarkRep::addPoint(QString uid)
{
  if (!this->exists(uid))
    return;

  vtkRendererPtr renderer;
  if (!mViews.empty())
    renderer = (*mViews.begin())->getRenderer();

  LandmarkGraphics current;

  std::map<QString, ssc::LandmarkProperty> props = ssc::dataManager()->getLandmarkProperties();
  QString name = props[uid].getName();

  current.mPoint.reset(new ssc::GraphicalPoint3D(renderer));
  current.mPoint->setColor(mColor);
  current.mPoint->setRadius(2);

  if (mShowLabel)
  {
    current.mText.reset(new ssc::FollowerText3D(renderer));
    current.mText->setText(name);
    current.mText->setSizeInNormalizedViewport(true, 0.025);
    current.mText->setColor(mColor);
  }

  if (mShowLine)
  {
    current.mLine.reset(new ssc::GraphicalLine3D(renderer));
    current.mLine->setColor(mColor);
//    current.mLine->setColor(ssc::Vector3D(1,0,0));
    current.mLine->setStipple(0x0F0F);
//    current.mLine->setRadius(2);
  }

  mGraphics[uid] = current;
//  this->setPosition(coord, uid);
}

void LandmarkRep::internalUpdate()
{
  std::map<QString, ssc::LandmarkProperty> props = ssc::dataManager()->getLandmarkProperties();

  for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
  {
    QString uid = iter->first;
    QString name = props[uid].getName();
    if (iter->second.mText)
    {
      iter->second.mText->setColor(mColor);
      iter->second.mText->setText(name);
      // a bit smaller than the corresponding vtkCaptionActor2D, because the sizes
      // are set differently.
      iter->second.mText->setSize(mLabelSize/100*0.75);
    }
    if (iter->second.mPoint)
    {
      iter->second.mPoint->setColor(mColor);
//      iter->second.mPoint->setRadius(mGraphicsSize);
    }
  }
  this->rescale();
}

void LandmarkRep::rescale()
{
	if (!mViewportListener->isListening())
		return;
	double size = mViewportListener->getVpnZoom();
  double sphereSize = mGraphicsSize/100/size;

  for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
  {
    if (iter->second.mPoint)
    {
      iter->second.mPoint->setRadius(sphereSize);
    }
  }

}


}//namespace cx
