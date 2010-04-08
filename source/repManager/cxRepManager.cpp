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
    addRep<InriaRep3D>(new InriaRep3D(mInriaRep3DNames[i],mInriaRep3DNames[i]), &mInriaRep3DMap);
  }
  for(int i=0; i<MAX_INRIAREP2DS; i++)
  {
    InriaRep2DPtr inriaRep2D = addRep<InriaRep2D>(new InriaRep2D(mInriaRep2DNames[i], mInriaRep2DNames[i]), &mInriaRep2DMap);
    //Remove text from Inria reps
    inriaRep2D->getVtkViewImage2D()->SetShowAnnotations(false);
    inriaRep2D->getVtkViewImage2D()->UpdateAnnotations();
    connect(inriaRep2D.get(), SIGNAL(pointPicked(double,double,double)),
            this, SLOT(syncInria2DRepsSlot(double,double,double)));
  }
  for(int i=0; i<MAX_VOLUMETRICREPS; i++)
  {
    addRep<ssc::VolumetricRep>(mVolumetricRepNames[i], &mVolumetricRepMap);
  }
  for(int i=0; i<MAX_PROGRESSIVEVOLUMETRICREPS; i++)
  {
    addRep<ProgressiveVolumetricRep>(mProgressiveVolumetricRepNames[i], &mProgressiveVolumetricRepMap);
  }
  for(int i=0; i<MAX_PROBEREPS; i++)
  {
    ProbeRepPtr probeRep = addRep<ProbeRep>(mProbeRepNames[i], &mProbeRepMap);
    connect(probeRep.get(), SIGNAL(pointPicked(double,double,double)),this, SLOT(probeRepPointPickedSlot(double,double,double)));
  }
  for(int i=0; i<MAX_LANDMARKREPS; i++)
  {
    addRep<LandmarkRep>(mLandmarkRepNames[i], &mLandmarkRepMap);
  }
  for(int i=0; i<MAX_TOOLREP3DS; i++)
  {
    addRep<ssc::ToolRep3D>(mToolRep3DNames[i], &mToolRep3DMap);
  }
  for(int i=0; i<MAX_GEOMETRICREPS; i++)
  {
    addRep<ssc::GeometricRep>(mGeometricRepNames[i], &mGeometricRepMap);
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

/**Shortcut function for adding new reps to the internal maps
 */
template<class REP, class MAP>
boost::shared_ptr<REP> RepManager::addRep(const std::string& uid, MAP* specificMap)
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
  ssc::Vector3D p_pr = ToolManager::getInstance()->get_rMpr()->inv().coord(p_r);
  // TODO set center here will not do: must handle
  DataManager::getInstance()->setCenter(p_r);
  ToolManager::getInstance()->getManualTool()->set_prMt(ssc::createTransformTranslate(p_pr));
}

RepManager::~RepManager()
{
}

std::vector<std::pair<std::string, std::string> > RepManager::getRepUidsAndNames()
{
  std::vector<std::pair<std::string, std::string> > retval;
  for(RepMap::iterator it = mRepMap.begin(); it != mRepMap.end(); ++it)
  {
    retval.push_back(make_pair(it->second->getUid(), it->second->getName()));
  }
  return retval;
}

RepMap* RepManager::getReps()
{
  return new RepMap(mRepMap);
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
  if (mRepMap.count(uid))
    return mRepMap[uid];
  return ssc::RepPtr();
}

template<class REP, class MAP>
boost::shared_ptr<REP> RepManager::getRep(const std::string& uid, MAP* specificMap)
{
  typename MAP::iterator iter = specificMap->find(uid);
  if (iter != specificMap->end())
    return iter->second;
  else
    return boost::shared_ptr<REP>();
}

InriaRep3DPtr RepManager::getInria3DRep(const std::string& uid)
{
  return getRep<InriaRep3D>(uid, &mInriaRep3DMap);
}
InriaRep2DPtr RepManager::getInria2DRep(const std::string& uid)
{
  return getRep<InriaRep2D>(uid, &mInriaRep2DMap);
}
ssc::VolumetricRepPtr RepManager::getVolumetricRep(const std::string& uid)
{
  return getRep<ssc::VolumetricRep>(uid, &mVolumetricRepMap);
}
ProgressiveVolumetricRepPtr RepManager::getProgressiveVolumetricRep(const std::string& uid)
{
  return getRep<ProgressiveVolumetricRep>(uid, &mProgressiveVolumetricRepMap);
}
ProbeRepPtr RepManager::getProbeRep(const std::string& uid)
{
  return getRep<ProbeRep>(uid, &mProbeRepMap);
}
LandmarkRepPtr RepManager::getLandmarkRep(const std::string& uid)
{
  return getRep<LandmarkRep>(uid, &mLandmarkRepMap);
}
ssc::ToolRep3DPtr RepManager::getToolRep3DRep(const std::string& uid)
{
  return getRep<ssc::ToolRep3D>(uid, &mToolRep3DMap);
}
ssc::GeometricRepPtr RepManager::getGeometricRep(const std::string& uid)
{
  return getRep<ssc::GeometricRep>(uid, &mGeometricRepMap);
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
