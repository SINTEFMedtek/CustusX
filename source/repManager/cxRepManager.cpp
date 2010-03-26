#include "cxRepManager.h"

#include "cxMessageManager.h"
#include "cxToolManager.h"
#include "cxDataManager.h"

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
{}
RepManager::RepManager() :
  MAX_INRIAREP3DS(2),
  MAX_INRIAREP2DS(9),
  MAX_VOLUMETRICREPS(2),
  MAX_PROGRESSIVEVOLUMETRICREPS(2),
  MAX_PROBEREPS(2),
  MAX_LANDMARKREPS(2),
  MAX_TOOLREP3DS(5),
  MAX_GEOMETRICREPS(6)
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

  mProgressiveVolumetricRepNames[0] = "ProgressiveVolumetricRep_1";
  mProgressiveVolumetricRepNames[1] = "ProgressiveVolumetricRep_2";

  mProbeRepNames[0] = "ProbeRep_1";
  mProbeRepNames[1] = "ProbeRep_2";

  mLandmarkRepNames[0] = "LandmarkRep_1";
  mLandmarkRepNames[1] = "LandmarkRep_2";

  mToolRep3DNames[0] = "ToolRep3D_1";
  mToolRep3DNames[1] = "ToolRep3D_2";
  mToolRep3DNames[2] = "ToolRep3D_3";
  mToolRep3DNames[3] = "ToolRep3D_4";
  mToolRep3DNames[4] = "ToolRep3D_5";

  mGeometricRepNames[0] = "GeometricRep_1";
  mGeometricRepNames[1] = "GeometricRep_2";
  mGeometricRepNames[2] = "GeometricRep_3";
  mGeometricRepNames[3] = "GeometricRep_4";
  mGeometricRepNames[4] = "GeometricRep_5";
  mGeometricRepNames[5] = "GeometricRep_6";

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
    //Remove text from Inria reps
    inriaRep2D->getVtkViewImage2D()->SetShowAnnotations(false);
    inriaRep2D->getVtkViewImage2D()->UpdateAnnotations();
    mInriaRep2DMap[inriaRep2D->getUid()] = inriaRep2D;
    connect(inriaRep2D.get(), SIGNAL(pointPicked(double,double,double)),
            this, SLOT(syncInria2DRepsSlot(double,double,double)));
  }
  for(int i=0; i<MAX_VOLUMETRICREPS; i++)
  {
    ssc::VolumetricRepPtr volumetricRep(ssc::VolumetricRep::New(mVolumetricRepNames[i],
        mVolumetricRepNames[i]));
    mVolumetricRepMap[volumetricRep->getUid()] = volumetricRep;
  }
  for(int i=0; i<MAX_PROGRESSIVEVOLUMETRICREPS; i++)
  {
    ProgressiveVolumetricRepPtr progressiveVolumetricRep(
        ProgressiveVolumetricRep::New(mProgressiveVolumetricRepNames[i],
        mProgressiveVolumetricRepNames[i]));
    mProgressiveVolumetricRepMap[progressiveVolumetricRep->getUid()] = progressiveVolumetricRep;
  }
  for(int i=0; i<MAX_PROBEREPS; i++)
  {
    ProbeRepPtr probeRep(ProbeRep::New(mProbeRepNames[i],
        mProbeRepNames[i]));
    mProbeRepMap[probeRep->getUid()] = probeRep;

    connect(probeRep.get(), SIGNAL(pointPicked(double,double,double)),this, SLOT(probeRepPointPickedSlot(double,double,double)));

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
  for(int i=0; i<MAX_GEOMETRICREPS; i++)
  {
    ssc::GeometricRepPtr geometricRep(ssc::GeometricRep::New(mGeometricRepNames[i],
        mGeometricRepNames[i]));
    mGeometricRepMap[geometricRep->getUid()] = geometricRep;
  }
  messageManager()->sendInfo("All necessary representations have been created.");

  /*//connect the two acs-sets so both get updated when we click on one of them
  connect(&(*getInria2DRep(mInriaRep2DNames[0])), SIGNAL(pointPicked(double,double,double)),
          this, SLOT(syncInria2DRepsSlot(double,double,double)));
  connect(&(*getInria2DRep(mInriaRep2DNames[3])), SIGNAL(pointPicked(double,double,double)),
            this, SLOT(syncInria2DRepsSlot(double,double,double)));*/

  //connect the dominant tool to the acs-sets so they also get update when we move the tool
  connect(ToolManager::getInstance(), SIGNAL(dominantToolChanged(const std::string&)),
          this, SLOT(dominantToolChangedSlot(const std::string&)));
}

void RepManager::probeRepPointPickedSlot(double x,double y,double z)
{
  //TODO check spaces....
  ssc::Vector3D p_r(x,y,z); // assume p is in r ...?
  ssc::Vector3D p_pr = ToolManager::getInstance()->get_rMpr()->inv().coord(p_r);
  // TODO set center here will not do: must handle
  DataManager::getInstance()->setCenter(p_r);
  ToolManager::getInstance()->getManualTool()->set_prMt(ssc::createTransformTranslate(p_pr));
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
  for(ProgressiveVolumetricRepMap::iterator it = mProgressiveVolumetricRepMap.begin(); it != mProgressiveVolumetricRepMap.end(); it++)
  {
    uidsAndNames->push_back(std::pair<std::string, std::string>(it->second->getUid(), it->second->getName()));
  }
  for(ProbeRepMap::iterator it = mProbeRepMap.begin(); it != mProbeRepMap.end(); it++)
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
  for(GeometricRepMap::iterator it = mGeometricRepMap.begin(); it != mGeometricRepMap.end(); it++)
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
  for(ProgressiveVolumetricRepMap::iterator it = mProgressiveVolumetricRepMap.begin(); it != mProgressiveVolumetricRepMap.end(); it++)
  {
    repmap->insert(std::pair<std::string, ssc::RepPtr>(it->first, it->second));
  }
  for(ProbeRepMap::iterator it = mProbeRepMap.begin(); it != mProbeRepMap.end(); it++)
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
  for(GeometricRepMap::iterator it = mGeometricRepMap.begin(); it != mGeometricRepMap.end(); it++)
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
ProgressiveVolumetricRepMap* RepManager::getProgressiveVolumetricReps()
{
  return &mProgressiveVolumetricRepMap;
}
ProbeRepMap* RepManager::getProbeReps()
{
  return &mProbeRepMap;
}
LandmarkRepMap* RepManager::getLandmarkReps()
{
  return &mLandmarkRepMap;
}
ToolRep3DMap* RepManager::getToolRep3DReps()
{
  return &mToolRep3DMap;
}
GeometricRepMap* RepManager::getGeometricReps()
{
  return &mGeometricRepMap;
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
  ProgressiveVolumetricRepMap::iterator it4 = mProgressiveVolumetricRepMap.find(uid);
  if(it4 != mProgressiveVolumetricRepMap.end())
    return it4->second;
  ProbeRepMap::iterator it5 = mProbeRepMap.find(uid);
  if(it5 != mProbeRepMap.end())
    return it5->second;
  LandmarkRepMap::iterator it6 = mLandmarkRepMap.find(uid);
  if(it6 != mLandmarkRepMap.end())
    return it6->second;
  ToolRep3DMap::iterator it7 = mToolRep3DMap.find(uid);
  if(it7 != mToolRep3DMap.end())
    return it7->second;
  GeometricRepMap::iterator it8 = mGeometricRepMap.find(uid);
  if(it8 != mGeometricRepMap.end())
    return it8->second;

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
ssc::VolumetricRepPtr RepManager::getVolumetricRep(const std::string& uid)
{
  VolumetricRepMap::iterator it = mVolumetricRepMap.find(uid);
  if(it != mVolumetricRepMap.end())
    return it->second;
  else
    return ssc::VolumetricRepPtr();
}
ProgressiveVolumetricRepPtr RepManager::getProgressiveVolumetricRep(const std::string& uid)
{
  ProgressiveVolumetricRepMap::iterator it = mProgressiveVolumetricRepMap.find(uid);
  if(it != mProgressiveVolumetricRepMap.end())
    return it->second;
  else
    return ProgressiveVolumetricRepPtr();
}
ProbeRepPtr RepManager::getProbeRep(const std::string& uid)
{
  ProbeRepMap::iterator it = mProbeRepMap.find(uid);
  if(it != mProbeRepMap.end())
    return it->second;
  else
    return ProbeRepPtr();
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
ssc::GeometricRepPtr RepManager::getGeometricRep(const std::string& uid)
{
  GeometricRepMap::iterator it = mGeometricRepMap.find(uid);
  if(it != mGeometricRepMap.end())
    return it->second;
  else
    return ssc::GeometricRepPtr();
}
void RepManager::syncInria2DRepsSlot(double x,double y,double z)
{
  const double point[3] = {x,y,z};
  getInria2DRep(mInriaRep2DNames[0])->getVtkViewImage2D()->SyncSetPosition(point);
  getInria2DRep(mInriaRep2DNames[3])->getVtkViewImage2D()->SyncSetPosition(point);
}
void RepManager::dominantToolChangedSlot(const std::string& toolUid)
{
  ssc::ToolPtr dominantTool = ToolManager::getInstance()->getDominantTool();
  if(!dominantTool)
  {
    messageManager()->sendError("Couldn't find a dominant tool to connect the inria2Dreps to.");
    return;
  }
  if(mConnectedTool == dominantTool)
  {
	  messageManager()->sendWarning("The new dominant tool was the same as the old one.");
    return;
  }
  if(mConnectedTool)
  {
    disconnect(mConnectedTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
               this, SLOT(receiveToolTransfromAndTimeStampSlot(Transform3D, double)));
  }
  mConnectedTool = dominantTool;
  connect(mConnectedTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
          this, SLOT(receiveToolTransfromAndTimeStampSlot(Transform3D, double)));


}
void RepManager::receiveToolTransfromAndTimeStampSlot(Transform3D prMt, double timestamp)
{
  //move the incoming transform into dataref space
  ssc::Transform3DPtr rMt(new Transform3D((*ToolManager::getInstance()->get_rMpr())*prMt));
  double x = (*rMt)[0][3];
  double y = (*rMt)[1][3];
  double z = (*rMt)[2][3];
  this->syncInria2DRepsSlot(x,y,z);
}
RepManager* repManager()
{
  return RepManager::getInstance();
}
}//namespace cx
