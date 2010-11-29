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

  ToolManager* toolmanager = ToolManager::getInstance();
  connect(toolmanager, SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkAddedSlot(QString)));
  connect(toolmanager, SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkRemovedSlot(QString)));
  connect(toolmanager, SIGNAL(rMprChanged()), this, SLOT(transformChangedSlot()));

  this->setTextScale(10,10,10);
}

PatientLandmarkRep::~PatientLandmarkRep()
{}

QString PatientLandmarkRep::getType() const
{
  return mType;
}

void PatientLandmarkRep::landmarkAddedSlot(QString uid)
{
  ToolManager* toolmanager = ToolManager::getInstance();
  ssc::Landmark landmark_pr = toolmanager->getLandmarks()[uid];
  ssc::Vector3D p_r = toolmanager->get_rMpr()->coord(landmark_pr.getCoord());// p_r = point in ref space
  this->addPoint(p_r, uid);
  //std::cout << "LandmarkRep::landmarkAddedSlot(" << uid << ") " << this->getUid() << std::endl;
}

void PatientLandmarkRep::transformChangedSlot()
{
  //std::cout << "LandmarkRep::transformChangedSlot()" << std::endl;

  ToolManager* toolmanager = ToolManager::getInstance();
  ssc::LandmarkMap landmarksMap = toolmanager->getLandmarks();
  ssc::LandmarkMap::iterator it = landmarksMap.begin();
  for(;it != landmarksMap.end();++it)
  {
    //this->landmarkAddedSlot(it->first);
    ssc::Landmark landmark_pr = it->second;
    ssc::Vector3D p_r = toolmanager->get_rMpr()->coord(landmark_pr.getCoord()); // p_r = point in ref space
    this->addPoint(p_r, it->first);
//    std::cout << "LandmarkRep::landmarkAddedSlot(" << uid << ") " << this->getUid() << std::endl;
  }
}

void PatientLandmarkRep::clearAll()
{
  ToolManager* toolmanager = ToolManager::getInstance();
  ssc::LandmarkMap landmarksMap = toolmanager->getLandmarks();
  ssc::LandmarkMap::iterator it = landmarksMap.begin();
  for(;it != landmarksMap.end();++it)
  {
    this->landmarkRemovedSlot(it->first);
  }

  mSkinPointActors.clear();
  mTextFollowerActors.clear();
}

void PatientLandmarkRep::addAll()
{
  ToolManager* toolmanager = ToolManager::getInstance();
  ssc::LandmarkMap landmarksMap = toolmanager->getLandmarks();
  ssc::LandmarkMap::iterator it = landmarksMap.begin();
  for(;it != landmarksMap.end();++it)
  {
    this->landmarkAddedSlot(it->first);
  }
}

void PatientLandmarkRep::setPosition(ssc::Vector3D coord, QString uid)
{
  ssc::Vector3D numberPosition = coord;

  mTextFollowerActors[uid].second->SetPosition(numberPosition.begin());
  mSkinPointActors[uid]->SetPosition(coord.begin());
}

}//namespace cx
