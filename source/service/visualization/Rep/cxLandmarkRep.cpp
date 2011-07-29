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

namespace cx
{

LandmarkRep::LandmarkRep(const QString& uid, const QString& name) :
  RepImpl(uid, name),
  mColor(0,1,0),
  mShowLandmarks(true),
  mShowLabel(true),
  mShowLine(false)

{
  mTextScale[0] = mTextScale[1] = mTextScale[2] = 20;
  connect(ssc::dataManager(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(internalUpdate()));
}

LandmarkRep::~LandmarkRep()
{}

void LandmarkRep::setColor(ssc::Vector3D color)
{
  mColor = color;
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
  }
  mShowLandmarks = on;
}

void LandmarkRep::landmarkAddedSlot(QString uid)
{
  this->addPoint(uid);
  this->setPosition(uid);
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
  }
}

void LandmarkRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
  {
    iter->second.mPoint->setRenderer(NULL);
    if (iter->second.mText)
    	iter->second.mText->setRenderer(NULL);
  }
}

/**
 * Use the inpup coord in ref space to render a landmark.
 */
void LandmarkRep::addPoint(QString uid)
{
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
    iter->second.mPoint->setColor(mColor);
    iter->second.mText->setColor(mColor);
    iter->second.mText->setText(name);
  }
}

}//namespace cx
