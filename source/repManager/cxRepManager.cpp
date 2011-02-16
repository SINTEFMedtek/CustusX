#include "cxRepManager.h"

#include "sscMessageManager.h"
#include "cxToolManager.h"
#include "sscDataManager.h"

#include "sscImage.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscVolumetricRep.h"
#include "sscProbeRep.h"
#include "sscGeometricRep.h"
#include "sscProgressiveLODVolumetricRep.h"
#include "cxTool.h"
//#include "cxImageLandmarkRep.h"
#include "cxDataLocations.h"
#include <QSettings>

namespace cx
{
RepManager* RepManager::mTheInstance = NULL;

RepManager* RepManager::getInstance()
{
  if(mTheInstance == NULL)
   {
     mTheInstance = new RepManager();
   }
   return mTheInstance;
}
void RepManager::destroyInstance()
{
  delete mTheInstance;
  mTheInstance = NULL;
}
RepManager::RepManager() :
  MAX_VOLUMETRICREPS(2),
  MAX_PROGRESSIVEVOLUMETRICREPS(2),
  MAX_PROBEREPS(2),
  MAX_LANDMARKREPS(2),
//  MAX_TOOLREP3DS(5),
  MAX_GEOMETRICREPS(6)
{
  mVolumetricRepNames[0] = "VolumetricRep_1";
  mVolumetricRepNames[1] = "VolumetricRep_2";

  mProgressiveVolumetricRepNames[0] = "ProgressiveVolumetricRep_1";
  mProgressiveVolumetricRepNames[1] = "ProgressiveVolumetricRep_2";

  mProbeRepNames[0] = "ProbeRep_1";
  mProbeRepNames[1] = "ProbeRep_2";

//  mLandmarkRepNames[0] = "LandmarkRep_1";
//  mLandmarkRepNames[1] = "LandmarkRep_2";

//  mToolRep3DNames[0] = "ToolRep3D_1";
//  mToolRep3DNames[1] = "ToolRep3D_2";
//  mToolRep3DNames[2] = "ToolRep3D_3";
//  mToolRep3DNames[3] = "ToolRep3D_4";
//  mToolRep3DNames[4] = "ToolRep3D_5";

  mGeometricRepNames[0] = "GeometricRep_1";
  mGeometricRepNames[1] = "GeometricRep_2";
  mGeometricRepNames[2] = "GeometricRep_3";
  mGeometricRepNames[3] = "GeometricRep_4";
  mGeometricRepNames[4] = "GeometricRep_5";
  mGeometricRepNames[5] = "GeometricRep_6";

  for(int i=0; i<MAX_VOLUMETRICREPS; i++)
  {
    addRep<ssc::VolumetricRep>(mVolumetricRepNames[i], &mVolumetricRepMap);
  }
  for(int i=0; i<MAX_PROGRESSIVEVOLUMETRICREPS; i++)
  {
    addRep<ssc::ProgressiveLODVolumetricRep>(mProgressiveVolumetricRepNames[i], &mProgressiveVolumetricRepMap);
  }
  for(int i=0; i<MAX_PROBEREPS; i++)
  {
    ssc::ProbeRepPtr probeRep = addRep<ssc::ProbeRep>(mProbeRepNames[i], &mProbeRepMap);
    connect(probeRep.get(), SIGNAL(pointPicked(double,double,double)),this, SLOT(probeRepPointPickedSlot(double,double,double)));
  }
//  for(int i=0; i<MAX_LANDMARKREPS; i++)
//  {
//    addRep<ImageLandmarkRep>(mLandmarkRepNames[i], &mLandmarkRepMap);
//  }
//  for(int i=0; i<MAX_TOOLREP3DS; i++)
//  {
//    addRep<ssc::ToolRep3D>(mToolRep3DNames[i], &mToolRep3DMap);
//  }
  for(int i=0; i<MAX_GEOMETRICREPS; i++)
  {
    addRep<ssc::GeometricRep>(mGeometricRepNames[i], &mGeometricRepMap);
  }
  ssc::messageManager()->sendInfo("All necessary representations have been created.");

  //connect the dominant tool to the acs-sets so they also get update when we move the tool
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)),
          this, SLOT(dominantToolChangedSlot(const QString&)));

  connect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SLOT(volumeRemovedSlot(QString)));
}

/**Shortcut function for adding new reps to the internal maps
 */
template<class REP, class MAP>
boost::shared_ptr<REP> RepManager::addRep(const QString& uid, MAP* specificMap)
{
  return addRep(REP::New(uid, uid), specificMap);
}

/**Shortcut function for adding new reps to the internal maps
 */
template<class REP, class MAP>
boost::shared_ptr<REP> RepManager::addRep(REP* raw, MAP* specificMap)
{
  return addRep(boost::shared_ptr<REP>(raw), specificMap);
}

/**Shortcut function for adding new reps to the internal maps
 */
template<class REP, class MAP>
boost::shared_ptr<REP> RepManager::addRep(boost::shared_ptr<REP> rep, MAP* specificMap)
{
  (*specificMap)[rep->getUid()] = rep;
  mRepMap[rep->getUid()] = rep;
  return rep;
}

void RepManager::probeRepPointPickedSlot(double x,double y,double z)
{
  //TODO check spaces....
  ssc::Vector3D p_r(x,y,z); // assume p is in r ...?
  ssc::Vector3D p_pr = ssc::toolManager()->get_rMpr()->inv().coord(p_r);
  // TODO set center here will not do: must handle
  ssc::dataManager()->setCenter(p_r);
  ToolManager::getInstance()->getManualTool()->set_prMt(ssc::createTransformTranslate(p_pr));
}

RepManager::~RepManager()
{
}

std::vector<std::pair<QString, QString> > RepManager::getRepUidsAndNames()
{
  std::vector<std::pair<QString, QString> > retval;
  for(RepMap::iterator it = mRepMap.begin(); it != mRepMap.end(); ++it)
  {
    retval.push_back(std::make_pair(it->second->getUid(), it->second->getName()));
  }
  return retval;
}

RepMap* RepManager::getReps()
{
  return new RepMap(mRepMap);
}

VolumetricRepMap* RepManager::getVolumetricReps()
{
  return &mVolumetricRepMap;
}

ProgressiveVolumetricRepMap* RepManager::getProgressiveVolumetricReps()
{
  return &mProgressiveVolumetricRepMap;
}

ProbeRepMap* RepManager::getProbeReps()
{
  return &mProbeRepMap;
}

//LandmarkRepMap* RepManager::getLandmarkReps()
//{
//  return &mLandmarkRepMap;
//}

//ToolRep3DMap* RepManager::getToolRep3DReps()
//{
//  return &mToolRep3DMap;
//}

GeometricRepMap* RepManager::getGeometricReps()
{
  return &mGeometricRepMap;
}

ssc::RepPtr RepManager::getRep(const QString& uid)
{
  if (mRepMap.count(uid))
    return mRepMap[uid];
  return ssc::RepPtr();
}

template<class REP, class MAP>
boost::shared_ptr<REP> RepManager::getRep(const QString& uid, MAP* specificMap)
{
  typename MAP::iterator iter = specificMap->find(uid);
  if (iter != specificMap->end())
    return iter->second;
  else
    return boost::shared_ptr<REP>();
}

ssc::VolumetricRepPtr RepManager::getVolumetricRep(const QString& uid)
{
  return getRep<ssc::VolumetricRep>(uid, &mVolumetricRepMap);
}

ssc::ProgressiveLODVolumetricRepPtr RepManager::getProgressiveVolumetricRep(const QString& uid)
{
  return getRep<ssc::ProgressiveLODVolumetricRep>(uid, &mProgressiveVolumetricRepMap);
}
ssc::ProbeRepPtr RepManager::getProbeRep(const QString& uid)
{
  return getRep<ssc::ProbeRep>(uid, &mProbeRepMap);
}
//LandmarkRepPtr RepManager::getLandmarkRep(const QString& uid)
//{
//  return getRep<LandmarkRep>(uid, &mLandmarkRepMap);
//}
//ssc::ToolRep3DPtr RepManager::getToolRep3DRep(const QString& uid)
//{
//  return getRep<ssc::ToolRep3D>(uid, &mToolRep3DMap);
//}
ssc::GeometricRepPtr RepManager::getGeometricRep(const QString& uid)
{
  return getRep<ssc::GeometricRep>(uid, &mGeometricRepMap);
}

void RepManager::dominantToolChangedSlot(const QString& toolUid)
{
  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
  if(!dominantTool)
  {
    ssc::messageManager()->sendError("Couldn't find a dominant tool to connect the inria2Dreps to.");
    return;
  }
  if(mConnectedTool == dominantTool)
    return;

  mConnectedTool = dominantTool;

}

/** return a rep with the given uid,
 *  if not found, create and return
 *
 */
//ssc::ToolRep3DPtr RepManager::getDynamicToolRep3DRep(QString uid)
//{
//  ssc::ToolRep3DPtr rep = this->getToolRep3DRep(uid);
//  if (!rep)
//  {
//    rep = ssc::ToolRep3D::New(uid);
//    this->addRep(rep, &mToolRep3DMap);
//  }
//  return rep;
//
//  //      mToolReps[uid] = ssc::ToolRep3D::New(uid);
//  //      repManager()->addToolRep3D(mToolReps[uid]);
//}


//void RepManager::addToolRep3D(ssc::ToolRep3DPtr rep)
//{
//  if (!this->getToolRep3DRep(rep->getUid()))
//  {
//    this->addRep(rep, &mToolRep3DMap);
//  }
//}

ssc::VolumetricRepPtr RepManager::getVolumetricRep(ssc::ImagePtr image)
{
  if (!image)
    return ssc::VolumetricRepPtr();

  if (!mVolumetricRepByImageMap.count(image->getUid()))
  {
    QString uid("VolumetricRep_img_" + image->getUid());
    ssc::VolumetricRepPtr rep = ssc::VolumetricRep::New(uid, uid);

    bool ok = true;
    double maxRenderSize = DataLocations::getSettings()->value("maxRenderSize").toDouble(&ok);
    if (!ok)
      maxRenderSize = 10 * pow(10.0,6);

    bool useGPURender = DataLocations::getSettings()->value("useGPUVolumeRayCastMapper").toBool();
    if (useGPURender)
    	rep->setUseGPUVolumeRayCastMapper();
    else
    	rep->setUseVolumeTextureMapper();

    rep->setMaxVolumeSize(maxRenderSize);
    rep->setImage(image);
    mVolumetricRepByImageMap[image->getUid()] = rep;
    //mImageMapperMonitorMap[image->getUid()].reset(new ssc::ImageMapperMonitor(rep));
  }
  return mVolumetricRepByImageMap[image->getUid()];
}

/**always remove from cache after deleting a volume, because we _might_ import the same volume again,
 * with the _same_ uid.
 *
 */
void RepManager::volumeRemovedSlot(QString uid)
{
  mVolumetricRepByImageMap.erase(uid);
}


RepManager* repManager()
{
  return RepManager::getInstance();
}
}//namespace cx
