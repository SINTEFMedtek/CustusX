#include "cxRepManager.h"

#include "cxMessageManager.h"

/**
 * cxRepManager.cpp
 *
 * \brief
 *
 * \date Dec 10, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
namespace cx
{
RepManager::RepManager* RepManager::mTheInstance = NULL;

RepManager::RepManager* RepManager::getInstance()
{
  if(mTheInstance == NULL)
   {
     mTheInstance = new RepManager();
   }
   return mTheInstance;
}
void RepManager::destroyInstance()
{}
RepManager::RepManager() :
  mMessageManager(MessageManager::getInstance()),
  MAX_INRIAREP3DS(2),
  MAX_INRIAREP2DS(9),
  MAX_VOLUMETRICREPS(2),
  MAX_LANDMARKREPS(2),
  MAX_TOOLREP3DS(5)
{
  mInriaRep3DNames[0] = "InriaRep3D_1";
  mInriaRep3DNames[1] = "InriaRep3D_2";

  mInriaRep2DNames[0] = "InriaRep2D_1";
  mInriaRep2DNames[1] = "InriaRep2D_2";
  mInriaRep2DNames[2] = "InriaRep2D_3";
  mInriaRep2DNames[3] = "InriaRep2D_4";
  mInriaRep2DNames[4] = "InriaRep2D_5";
  mInriaRep2DNames[5] = "InriaRep2D_6";
  mInriaRep2DNames[6] = "InriaRep2D_7";
  mInriaRep2DNames[7] = "InriaRep2D_8";
  mInriaRep2DNames[8] = "InriaRep2D_9";

  mVolumetricRepNames[0] = "VolumetricRep_1";
  mVolumetricRepNames[1] = "VolumetricRep_2";

  mLandmarkRepNames[0] = "LandmarkRep_1";
  mLandmarkRepNames[1] = "LandmarkRep_2";

  mToolRep3DNames[0] = "ToolRep3D_1";
  mToolRep3DNames[1] = "ToolRep3D_2";
  mToolRep3DNames[2] = "ToolRep3D_3";
  mToolRep3DNames[3] = "ToolRep3D_4";
  mToolRep3DNames[4] = "ToolRep3D_5";

  for(int i=0; i<MAX_INRIAREP3DS; i++)
  {
    InriaRep3DPtr inriaRep3D(new InriaRep3D(mInriaRep3DNames[i],
        mInriaRep3DNames[i]));
    mInriaRep3DMap[inriaRep3D->getUid()] = inriaRep3D;
  }
  for(int i=0; i<MAX_INRIAREP2DS; i++)
  {
    InriaRep2DPtr inriaRep2D(new InriaRep2D(mInriaRep2DNames[i],
        mInriaRep2DNames[i]));
    mInriaRep2DMap[inriaRep2D->getUid()] = inriaRep2D;
  }
  for(int i=0; i<MAX_VOLUMETRICREPS; i++)
  {
    VolumetricRepPtr volumetricRep(VolumetricRep::New(mVolumetricRepNames[i],
        mVolumetricRepNames[i]));
    mVolumetricRepMap[volumetricRep->getUid()] = volumetricRep;
  }
  for(int i=0; i<MAX_LANDMARKREPS; i++)
  {
    LandmarkRepPtr landmarkRep(LandmarkRep::New(mLandmarkRepNames[i],
        mLandmarkRepNames[i]));
    mLandmarkRepMap[landmarkRep->getUid()] = landmarkRep;
  }
  for(int i=0; i<MAX_TOOLREP3DS; i++)
  {
    ssc::ToolRep3DPtr toolRep(ssc::ToolRep3D::New(mToolRep3DNames[i],
        mToolRep3DNames[i]));
    mToolRep3DMap[toolRep->getUid()] = toolRep;
  }
  mMessageManager.sendInfo("All necessary representations have been created.");
}
RepManager::~RepManager()
{}
std::vector<std::pair<std::string, std::string> > RepManager::getRepUidsAndNames()
{
  std::vector<std::pair<std::string, std::string> >* uidsAndNames = new std::vector<std::pair<std::string, std::string> >;
  for(InriaRep3DMap::iterator it = mInriaRep3DMap.begin(); it != mInriaRep3DMap.end(); it++)
  {
    uidsAndNames->push_back(std::pair<std::string, std::string>(it->second->getUid(), it->second->getName()));
  }
  for(InriaRep2DMap::iterator it = mInriaRep2DMap.begin(); it != mInriaRep2DMap.end(); it++)
  {
    uidsAndNames->push_back(std::pair<std::string, std::string>(it->second->getUid(), it->second->getName()));
  }
  for(VolumetricRepMap::iterator it = mVolumetricRepMap.begin(); it != mVolumetricRepMap.end(); it++)
  {
    uidsAndNames->push_back(std::pair<std::string, std::string>(it->second->getUid(), it->second->getName()));
  }
  for(LandmarkRepMap::iterator it = mLandmarkRepMap.begin(); it != mLandmarkRepMap.end(); it++)
  {
    uidsAndNames->push_back(std::pair<std::string, std::string>(it->second->getUid(), it->second->getName()));
  }
  for(ToolRep3DMap::iterator it = mToolRep3DMap.begin(); it != mToolRep3DMap.end(); it++)
  {
    uidsAndNames->push_back(std::pair<std::string, std::string>(it->second->getUid(), it->second->getName()));
  }
  return *uidsAndNames;
}
RepMap* RepManager::getReps()
{
  RepMap* repmap = new RepMap;
  for(InriaRep3DMap::iterator it = mInriaRep3DMap.begin(); it != mInriaRep3DMap.end(); it++)
  {
    repmap->insert(std::pair<std::string, ssc::RepPtr>(it->first, it->second));
  }
  for(InriaRep2DMap::iterator it = mInriaRep2DMap.begin(); it != mInriaRep2DMap.end(); it++)
  {
    repmap->insert(std::pair<std::string, ssc::RepPtr>(it->first, it->second));
  }
  for(VolumetricRepMap::iterator it = mVolumetricRepMap.begin(); it != mVolumetricRepMap.end(); it++)
  {
    repmap->insert(std::pair<std::string, ssc::RepPtr>(it->first, it->second));
  }
  for(LandmarkRepMap::iterator it = mLandmarkRepMap.begin(); it != mLandmarkRepMap.end(); it++)
  {
    repmap->insert(std::pair<std::string, ssc::RepPtr>(it->first, it->second));
  }
  for(ToolRep3DMap::iterator it = mToolRep3DMap.begin(); it != mToolRep3DMap.end(); it++)
  {
    repmap->insert(std::pair<std::string, ssc::RepPtr>(it->first, it->second));
  }
  return repmap;
}
InriaRep3DMap* RepManager::getInria3DReps()
{
  return &mInriaRep3DMap;
}
InriaRep2DMap* RepManager::getInria2DReps()
{
  return &mInriaRep2DMap;
}
VolumetricRepMap* RepManager::getVolumetricReps()
{
  return &mVolumetricRepMap;
}
LandmarkRepMap* RepManager::getLandmarkReps()
{
  return &mLandmarkRepMap;
}
ToolRep3DMap* RepManager::getToolRep3DReps()
{
  return &mToolRep3DMap;
}
ssc::RepPtr RepManager::getRep(const std::string& uid)
{
  InriaRep3DMap::iterator it1 = mInriaRep3DMap.find(uid);
  if(it1 != mInriaRep3DMap.end())
    return it1->second;
  InriaRep2DMap::iterator it2 = mInriaRep2DMap.find(uid);
  if(it2 != mInriaRep2DMap.end())
    return it2->second;
  VolumetricRepMap::iterator it3 = mVolumetricRepMap.find(uid);
  if(it3 != mVolumetricRepMap.end())
    return it3->second;
  LandmarkRepMap::iterator it4 = mLandmarkRepMap.find(uid);
  if(it4 != mLandmarkRepMap.end())
    return it4->second;
  ToolRep3DMap::iterator it5 = mToolRep3DMap.find(uid);
  if(it5 != mToolRep3DMap.end())
    return it5->second;

  return ssc::RepPtr();
}
InriaRep3DPtr RepManager::getInria3DRep(const std::string& uid)
{
  InriaRep3DMap::iterator it = mInriaRep3DMap.find(uid);
  if(it != mInriaRep3DMap.end())
    return it->second;
  else
    return InriaRep3DPtr();
}
InriaRep2DPtr RepManager::getInria2DRep(const std::string& uid)
{
  InriaRep2DMap::iterator it = mInriaRep2DMap.find(uid);
  if(it != mInriaRep2DMap.end())
    return it->second;
  else
    return InriaRep2DPtr();
}
VolumetricRepPtr RepManager::getVolumetricRep(const std::string& uid)
{
  VolumetricRepMap::iterator it = mVolumetricRepMap.find(uid);
  if(it != mVolumetricRepMap.end())
    return it->second;
  else
    return VolumetricRepPtr();
}
LandmarkRepPtr RepManager::getLandmarkRep(const std::string& uid)
{
  LandmarkRepMap::iterator it = mLandmarkRepMap.find(uid);
  if(it != mLandmarkRepMap.end())
    return it->second;
  else
    return LandmarkRepPtr();
}
ssc::ToolRep3DPtr RepManager::getToolRep3DRep(const std::string& uid)
{
  ToolRep3DMap::iterator it = mToolRep3DMap.find(uid);
  if(it != mToolRep3DMap.end())
    return it->second;
  else
    return ssc::ToolRep3DPtr();
}
void RepManager::receivePointToSyncSlot(double x, double y, double z)
{
  //TODO
}
}//namespace cx
