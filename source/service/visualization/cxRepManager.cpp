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
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscVolumetricRep.h"
#include "sscGeometricRep.h"
#include "sscProgressiveLODVolumetricRep.h"
#include "cxSettings.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxThresholdPreview.h"

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
//	mIsUsingGPU3DMapper = false;
//	mMaxRenderSize = 0;
	mThresholdPreview.reset(new ThresholdPreview());

//  connect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SLOT(volumeRemovedSlot(QString)));
}

RepManager::~RepManager()
{
}

ThresholdPreviewPtr RepManager::getThresholdPreview()
{
	return mThresholdPreview;
}

//ssc::VolumetricBaseRepPtr RepManager::getVolumetricRep(ssc::ImagePtr image)
//{
//  if (!image)
//	return ssc::VolumetricBaseRepPtr();

//  // clear cache if settings have changed
//  bool ok = true;
//  double maxRenderSize = settings()->value("maxRenderSize").toDouble(&ok);
//  if (!ok)
//    maxRenderSize = 10 * pow(10.0,6);

//  bool useGPURender = settings()->value("useGPUVolumeRayCastMapper").toBool();
//  bool useProgressiveLODTextureVolumeRayCastMapper = settings()->value("useProgressiveLODTextureVolumeRayCastMapper").toBool();

//  if (mIsUsingGPU3DMapper!=useGPURender || !ssc::similar(mMaxRenderSize, maxRenderSize))
//  {
//  	mIsUsingGPU3DMapper=useGPURender;
//  	mMaxRenderSize=maxRenderSize;
//  	mVolumetricRepByImageMap.clear();
//  }

//  if (!mVolumetricRepByImageMap.count(image->getUid()))
//  {
//	ssc::VolumetricBaseRepPtr rep;

//	if (useProgressiveLODTextureVolumeRayCastMapper && !useGPURender)
//	{
//		rep = ssc::ProgressiveLODVolumetricRep::New();
//	}
//	else
//	{
//		ssc::VolumetricRepPtr volrep = ssc::VolumetricRep::New();
//		if (useGPURender)
//		{
//			volrep->setUseGPUVolumeRayCastMapper();
//		}
//		else
//		{
//			volrep->setUseVolumeTextureMapper();
//		}
//		rep = volrep;
//	}

//    rep->setMaxVolumeSize(maxRenderSize);
//    rep->setImage(image);
//    mVolumetricRepByImageMap[image->getUid()] = rep;
//  }

//  //Call to purge is moved to ViewWrapper::dataRemovedSlot
//  //this->purgeVolumetricReps();

//  return mVolumetricRepByImageMap[image->getUid()];
//}




//void RepManager::purgeVolumetricReps()
//{
//	std::map<QString, ssc::ImagePtr> images = ViewManager::getInstance()->getVisibleImages();

//	VolumetricRepMap::const_iterator iter;
//	for (iter= mVolumetricRepByImageMap.begin(); iter != mVolumetricRepByImageMap.end(); ++iter)
//	{
//		//Remove from cache if not a visible image
//		if (!images.count(iter->first))
//		{
////			std::cout << "purge image from cache: " << iter->first << std::endl;
//			this->volumeRemovedSlot(iter->first);
//		}
//	}
//}

///**always remove from cache after deleting a volume, because we _might_ import the same volume again,
// * with the _same_ uid.
// *
// */
//void RepManager::volumeRemovedSlot(QString uid)
//{
//  mVolumetricRepByImageMap.erase(uid);

//  // all reps with ids that contains the volume uid will be cleared from the cache.
//	for (RepMultiMap::iterator iter=mRepCache.begin(); iter!= mRepCache.end();)
//	{
//		RepMultiMap::iterator last = iter++;

//		if (last->first.contains(uid))
//			mRepCache.erase(last);
//	}
//}


//RepManager* repManager()
//{
//  return RepManager::getInstance();
//}
}//namespace cx
