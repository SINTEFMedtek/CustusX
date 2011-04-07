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
#include "cxSettings.h"

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
RepManager::RepManager()
{
  connect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SLOT(volumeRemovedSlot(QString)));
}
//
///**Shortcut function for adding new reps to the internal maps
// */
//template<class REP, class MAP>
//boost::shared_ptr<REP> RepManager::addRep(const QString& uid, MAP* specificMap)
//{
//  return addRep(REP::New(uid, uid), specificMap);
//}
//
///**Shortcut function for adding new reps to the internal maps
// */
//template<class REP, class MAP>
//boost::shared_ptr<REP> RepManager::addRep(REP* raw, MAP* specificMap)
//{
//  return addRep(boost::shared_ptr<REP>(raw), specificMap);
//}
//
///**Shortcut function for adding new reps to the internal maps
// */
//template<class REP, class MAP>
//boost::shared_ptr<REP> RepManager::addRep(boost::shared_ptr<REP> rep, MAP* specificMap)
//{
//  (*specificMap)[rep->getUid()] = rep;
//  mRepMap[rep->getUid()] = rep;
//  return rep;
//}

RepManager::~RepManager()
{
}

//template<class REP, class MAP>
//boost::shared_ptr<REP> RepManager::getRep(const QString& uid, MAP* specificMap)
//{
//  typename MAP::iterator iter = specificMap->find(uid);
//  if (iter != specificMap->end())
//    return iter->second;
//  else
//    return boost::shared_ptr<REP>();
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
    double maxRenderSize = settings()->value("maxRenderSize").toDouble(&ok);
    if (!ok)
      maxRenderSize = 10 * pow(10.0,6);

    bool useGPURender = settings()->value("useGPUVolumeRayCastMapper").toBool();
    if (useGPURender)
    	rep->setUseGPUVolumeRayCastMapper();
    else
    	rep->setUseVolumeTextureMapper();

    rep->setMaxVolumeSize(maxRenderSize);
    rep->setImage(image);
    mVolumetricRepByImageMap[image->getUid()] = rep;
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
