// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxRepManager.h"

#include "sscMessageManager.h"
#include "cxToolManager.h"
#include "sscDataManager.h"

#include "sscImage.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscVolumetricRep.h"
//#include "sscPickerRep.h"
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
	mThresholdPreview.reset(new ThresholdPreview());

  connect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SLOT(volumeRemovedSlot(QString)));
}

RepManager::~RepManager()
{
}

ThresholdPreviewPtr RepManager::getThresholdPreview()
{
	return mThresholdPreview;
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
	for (RepMultiMap::iterator iter=mRepCache.begin(); iter!= mRepCache.end();)
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
