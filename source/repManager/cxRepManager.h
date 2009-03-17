#ifndef CXREPMANAGER_H_
#define CXREPMANAGER_H_

#include <QObject>
#include <map>
#include "sscToolRep3D.h"
#include "cxInriaRep3D.h"
#include "cxInriaRep2D.h"
#include "cxVolumetricRep.h"
#include "sscProgressiveLODVolumetricRep.h"
#include "cxLandmarkRep.h"

namespace cx
{
typedef ssc::ProgressiveLODVolumetricRep ProgressiveVolumetricRep;
typedef ssc::ProgressiveLODVolumetricRepPtr ProgressiveVolumetricRepPtr;

typedef std::map<std::string, ssc::RepPtr> RepMap;
typedef std::map<std::string, InriaRep2DPtr> InriaRep2DMap;
typedef std::map<std::string, InriaRep3DPtr> InriaRep3DMap;
typedef std::map<std::string, VolumetricRepPtr> VolumetricRepMap;
typedef std::map<std::string, ProgressiveVolumetricRepPtr> ProgressiveVolumetricRepMap;
typedef std::map<std::string, LandmarkRepPtr> LandmarkRepMap;
typedef std::map<std::string, ssc::ToolRep3DPtr> ToolRep3DMap;

class MessageManager;

/**
 * \class RepManager
 *
 * \brief Creates a pool of reps (representations) and offers an interface to
 * access them.
 *
 * \warning ProgressiveLODVolumetricRep does not work!
 *
 * \date Dec 10, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class RepManager : public QObject
{
  Q_OBJECT

public:
  static RepManager* getInstance(); ///< get the only instance of this class
  static void destroyInstance(); ///< destroy the only instance of this class

  std::vector<std::pair<std::string, std::string> > getRepUidsAndNames(); ///< get unique id and name of all reps in the pool

  RepMap* getReps(); ///< get all reps in the pool
  InriaRep3DMap* getInria3DReps(); ///< get all Inria3D reps in the pool
  InriaRep2DMap* getInria2DReps(); ///< get all Inria2D reps in the pool
  VolumetricRepMap* getVolumetricReps(); ///< get all Volumetric reps in the pool
  ProgressiveVolumetricRepMap* RepManager::getProgressiveVolumetricReps(); ///< get all ProgressiveLODVolumetric reps in the pool
  LandmarkRepMap* getLandmarkReps(); ///< get all Landmark reps in the pool
  ToolRep3DMap* getToolRep3DReps(); ///< get all Tool3D reps in the pool

  ssc::RepPtr getRep(const std::string& uid); ///< get one specific rep
  InriaRep3DPtr getInria3DRep(const std::string& uid); ///< get one specific Inria3D rep
  InriaRep2DPtr getInria2DRep(const std::string& uid); ///< get one specific Inria2D rep
  VolumetricRepPtr getVolumetricRep(const std::string& uid); ///< get one specific Volumetric rep
  ProgressiveVolumetricRepPtr getProgressiveVolumetricRep(const std::string& uid); ///< get one specific ProgressiveLODVolumetric rep
  LandmarkRepPtr getLandmarkRep(const std::string& uid); ///<  get one specific Landmark rep
  ssc::ToolRep3DPtr getToolRep3DRep(const std::string& uid); ///<  get one specific Tool3D rep

public slots:
  void receivePointToSyncSlot(double x, double y, double z); ///< this slot syncs inria reps when a point is picked in a view...

protected:
  static RepManager*  mTheInstance;         ///< the only instance of this class
  MessageManager*     mMessageManager;      ///< device for sending messages to the statusbar

  const int           MAX_INRIAREP3DS; ///< number of Inria3D reps in the pool
  std::string         mInriaRep3DNames[2]; ///< the name of the reps in the pool
  InriaRep3DMap       mInriaRep3DMap;     ///< the reps in the pool

  const int           MAX_INRIAREP2DS; ///< number of Inria2D reps in the pool
  std::string         mInriaRep2DNames[9]; ///< the name of the reps in the pool
  InriaRep2DMap       mInriaRep2DMap;     ///< the reps in the pool

  const int           MAX_VOLUMETRICREPS; ///< number of Volumetric reps in the pool
  std::string         mVolumetricRepNames[2]; ///< the name of the reps in the pool
  VolumetricRepMap    mVolumetricRepMap;  ///< the reps in the pool

  const int           MAX_PROGRESSIVEVOLUMETRICREPS; ///< number of ProgressiveLODVolumetric reps in the pool
  std::string         mProgressiveVolumetricRepNames[2]; ///< the name of the reps in the pool
  ProgressiveVolumetricRepMap    mProgressiveVolumetricRepMap;  ///< the reps in the pool

  const int           MAX_LANDMARKREPS; ///< number of Landmark reps in the pool
  std::string         mLandmarkRepNames[2]; ///< the name of the reps in the pool
  LandmarkRepMap      mLandmarkRepMap;    ///< the reps in the pool

  const int           MAX_TOOLREP3DS; ///< number of Tool3D reps in the pool
  std::string         mToolRep3DNames[5]; ///< the name of the reps in the pool
  ToolRep3DMap        mToolRep3DMap;      ///< the reps in the pool

private:
  RepManager(); ///< creates a pool of reps
  ~RepManager(); ///< empty
  RepManager(RepManager const&); ///< not implemented
  RepManager& operator=(RepManager const&); ///< not implemented
};
}//namespace cx
#endif /* CXREPMANAGER_H_ */
