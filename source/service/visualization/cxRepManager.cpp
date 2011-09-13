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
	mIsUsingGPU3DMapper = false;
	mMaxRenderSize = 0;

  connect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SLOT(volumeRemovedSlot(QString)));
}

RepManager::~RepManager()
{
}

ssc::VolumetricRepPtr RepManager::getVolumetricRep(ssc::ImagePtr image)
{
  if (!image)
    return ssc::VolumetricRepPtr();

  // clear cache if settings have changed
  bool ok = true;
  double maxRenderSize = settings()->value("maxRenderSize").toDouble(&ok);
  if (!ok)
    maxRenderSize = 10 * pow(10.0,6);

  bool useGPURender = settings()->value("useGPUVolumeRayCastMapper").toBool();

  if (mIsUsingGPU3DMapper!=useGPURender || !ssc::similar(mMaxRenderSize, maxRenderSize))
  {
  	mIsUsingGPU3DMapper=useGPURender;
  	mMaxRenderSize=maxRenderSize;
  	mVolumetricRepByImageMap.clear();
  }

  if (!mVolumetricRepByImageMap.count(image->getUid()))
  {
    QString uid("VolumetricRep_img_" + image->getUid());
    ssc::VolumetricRepPtr rep = ssc::VolumetricRep::New(uid, uid);

//    bool useGPURender = settings()->value("useGPUVolumeRayCastMapper").toBool();
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

  // all reps with ids that contains the volume uid will be cleared from the cache.
	for (RepMultiMap::iterator iter=mRepCache.begin(); iter!= mRepCache.end(); ++iter)
	{
		RepMultiMap::iterator last = iter++;

		if (last->first.contains(uid))
			mRepCache.erase(last);
	}
}


//RepManager* repManager()
//{
//  return RepManager::getInstance();
//}
}//namespace cx
