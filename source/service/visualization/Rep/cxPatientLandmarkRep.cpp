#include "cxPatientLandmarkRep.h"

#include <vtkFollower.h>
#include "sscCoordinateSystemHelpers.h"
#include "cxToolManager.h"

namespace cx
{
PatientLandmarkRepPtr PatientLandmarkRep::New(const QString& uid, const QString& name)
{
  PatientLandmarkRepPtr retval(new PatientLandmarkRep(uid, name));
  retval->mSelf = retval;
  return retval;
}

PatientLandmarkRep::PatientLandmarkRep(const QString& uid, const QString& name) :
  LandmarkRep(uid, name)
{
  mType = "cxPatientLandmarkRep";
  this->setColor(ssc::Vector3D(0,0.8,0.6));
  this->setShowLabel(false);
  this->setShowLine(false);

  ToolManager* toolmanager = ToolManager::getInstance();
  connect(toolmanager, SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkAddedSlot(QString)));
  connect(toolmanager, SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkRemovedSlot(QString)));
  connect(toolmanager, SIGNAL(rMprChanged()), this, SLOT(transformChangedSlot()));
}

PatientLandmarkRep::~PatientLandmarkRep()
{}

QString PatientLandmarkRep::getType() const
{
  return mType;
}

void PatientLandmarkRep::setImage(ssc::ImagePtr image)
{
  if(image == mImage)
    return;

  if(mImage)
  {
    mImage->disconnectFromRep(mSelf);
    disconnect(mImage.get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkAddedSlot(QString)));
    disconnect(mImage.get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkRemovedSlot(QString)));
    disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
    this->clearAll();
  }

  mImage = image;

  if(mImage)
  {
    mImage->connectToRep(mSelf);
    connect(mImage.get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkAddedSlot(QString)));
    connect(mImage.get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkRemovedSlot(QString)));
    connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
    this->addAll();
  }
}


void PatientLandmarkRep::addAll()
{
  ssc::LandmarkMap landmarksMap = ToolManager::getInstance()->getLandmarks();

  for(ssc::LandmarkMap::iterator it=landmarksMap.begin(); it!=landmarksMap.end(); ++it)
  {
    this->landmarkAddedSlot(it->first);
  }
}

void PatientLandmarkRep::setPosition(QString uid)
{
  ssc::Landmark landmark_pr = ToolManager::getInstance()->getLandmarks()[uid];
  ssc::Vector3D p_r = ToolManager::getInstance()->get_rMpr()->coord(landmark_pr.getCoord());// p_r = point in ref space

  mGraphics[uid].mPoint->setValue(p_r);

  if (mGraphics[uid].mText)
    mGraphics[uid].mText->setPosition(p_r);

  if (mImage && mGraphics[uid].mLine)
  {
    ssc::Landmark landmark = mImage->getLandmarks()[uid];
    ssc::Vector3D img_r = mImage->get_rMd().coord(landmark.getCoord()); // p_r = point in ref space
    mGraphics[uid].mLine->setValue(p_r, img_r);
  }

}

}//namespace cx
