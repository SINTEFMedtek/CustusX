#ifndef CXREPMANAGER_H_
#define CXREPMANAGER_H_

#include <QObject>
#include <map>
#include "sscToolRep3D.h"
#include "cxInriaRep3D.h"
#include "cxInriaRep2D.h"
#include "cxVolumetricRep.h"
#include "cxLandmarkRep.h"

/**
 * \class RepManager
 *
 * \brief
 *
 * \date Dec 10, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
namespace cx
{
typedef std::map<std::string, ssc::RepPtr> RepMap;
typedef std::map<std::string, InriaRep2DPtr> InriaRep2DMap;
typedef std::map<std::string, InriaRep3DPtr> InriaRep3DMap;
typedef std::map<std::string, VolumetricRepPtr> VolumetricRepMap;
typedef std::map<std::string, LandmarkRepPtr> LandmarkRepMap;
typedef std::map<std::string, ssc::ToolRep3DPtr> ToolRep3DMap;

class MessageManager;
class RepManager : public QObject
{
  Q_OBJECT

public:
  static RepManager* getInstance();
  static void destroyInstance();

  std::vector<std::pair<std::string, std::string> > getRepUidsAndNames();

  RepMap* getReps();
  InriaRep3DMap* getInria3DReps();
  InriaRep2DMap* getInria2DReps();
  VolumetricRepMap* getVolumetricReps();
  LandmarkRepMap* getLandmarkReps();
  ToolRep3DMap* getToolRep3DReps();

  ssc::RepPtr getRep(const std::string& uid);
  InriaRep3DPtr getInria3DRep(const std::string& uid);
  InriaRep2DPtr getInria2DRep(const std::string& uid);
  VolumetricRepPtr getVolumetricRep(const std::string& uid);
  LandmarkRepPtr getLandmarkRep(const std::string& uid);
  ssc::ToolRep3DPtr getToolRep3DRep(const std::string& uid);

public slots:
  void receivePointToSyncSlot(double x, double y, double z); ///< this slot syncs inria reps when a point is picked in a view...

protected:
  static RepManager*  mTheInstance;         ///< the only instance of this class
  MessageManager&     mMessageManager;      ///< device for sending messages to the statusbar

  const int           MAX_INRIAREP3DS;
  std::string         mInriaRep3DNames[2];
  InriaRep3DMap       mInriaRep3DMap;     ///<

  const int           MAX_INRIAREP2DS;
  std::string         mInriaRep2DNames[9];
  InriaRep2DMap       mInriaRep2DMap;     ///<

  const int           MAX_VOLUMETRICREPS;
  std::string         mVolumetricRepNames[2];
  VolumetricRepMap    mVolumetricRepMap;  ///<

  const int           MAX_LANDMARKREPS;
  std::string         mLandmarkRepNames[2];
  LandmarkRepMap      mLandmarkRepMap;    ///<

  const int           MAX_TOOLREP3DS;
  std::string         mToolRep3DNames[5];
  ToolRep3DMap        mToolRep3DMap;      ///<

private:
  RepManager();
  ~RepManager();
};
}//namespace cx
#endif /* CXREPMANAGER_H_ */
