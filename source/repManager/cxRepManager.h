#ifndef CXREPMANAGER_H_
#define CXREPMANAGER_H_

#include <QObject>
#include <map>
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscVolumetricRep.h"
#include "sscProbeRep.h"
#include "sscGeometricRep.h"
#include "sscProgressiveLODVolumetricRep.h"
#include "cxTool.h"
#include "cxLandmarkRep.h"

namespace cx
{
typedef ssc::ProbeRep ProbeRep;
typedef ssc::Transform3D Transform3D;
typedef ssc::ProbeRepPtr ProbeRepPtr;
typedef ssc::ProgressiveLODVolumetricRep ProgressiveVolumetricRep;
typedef ssc::ProgressiveLODVolumetricRepPtr ProgressiveVolumetricRepPtr;

typedef std::map<std::string, ssc::RepPtr> RepMap;
typedef std::map<std::string, ssc::VolumetricRepPtr> VolumetricRepMap;
typedef std::map<std::string, ProbeRepPtr> ProbeRepMap;
typedef std::map<std::string, ProgressiveVolumetricRepPtr> ProgressiveVolumetricRepMap;
typedef std::map<std::string, LandmarkRepPtr> LandmarkRepMap;
typedef std::map<std::string, ssc::ToolRep3DPtr> ToolRep3DMap;
typedef std::map<std::string, ssc::GeometricRepPtr> GeometricRepMap;

//typedef std::map<std::string, ssc::ImageMapperMonitorPtr> ImageMapperMonitorMap;

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
  VolumetricRepMap* getVolumetricReps(); ///< get all Volumetric reps in the pool
  ProbeRepMap* getProbeReps(); ///< get all Probe reps in the pool
  ProgressiveVolumetricRepMap* getProgressiveVolumetricReps(); ///< get all ProgressiveLODVolumetric reps in the pool
  LandmarkRepMap* getLandmarkReps(); ///< get all Landmark reps in the pool
  ToolRep3DMap* getToolRep3DReps(); ///< get all Tool3D reps in the pool
  GeometricRepMap* getGeometricReps(); ///< get all Geometric reps in the pool

  ssc::RepPtr getRep(const std::string& uid); ///< get one specific rep
  ssc::VolumetricRepPtr getVolumetricRep(const std::string& uid); ///< get one specific Volumetric rep
  ProbeRepPtr getProbeRep(const std::string& uid); ///< get one specific Probe rep
  ProgressiveVolumetricRepPtr getProgressiveVolumetricRep(const std::string& uid); ///< get one specific ProgressiveLODVolumetric rep
  LandmarkRepPtr getLandmarkRep(const std::string& uid); ///<  get one specific Landmark rep
  ssc::ToolRep3DPtr getToolRep3DRep(const std::string& uid); ///<  get one specific Tool3D rep
  ssc::GeometricRepPtr getGeometricRep(const std::string& uid); ///<  get one specific Geometric rep

//  void addToolRep3D(ssc::ToolRep3DPtr rep);
  ssc::ToolRep3DPtr getDynamicToolRep3DRep(std::string uid);

  /**Get a volumetric rep based on which image you want to  display.
   * This is useful when creating the rep is expensive and should be done only once.
   */
  ssc::VolumetricRepPtr getVolumetricRep(ssc::ImagePtr image);

protected slots:
  void probeRepPointPickedSlot(double x,double y,double z);
  void dominantToolChangedSlot(const std::string& toolUid); ///< makes sure the inriareps are connected to the right tool

protected:
  template<class REP, class MAP>
  boost::shared_ptr<REP> addRep(const std::string& uid, MAP* specificMap);
  template<class REP, class MAP>
  boost::shared_ptr<REP> addRep(REP* rep, MAP* specificMap);
  template<class REP, class MAP>
  boost::shared_ptr<REP> addRep(boost::shared_ptr<REP> rep, MAP* specificMap);
  template<class REP, class MAP>
  boost::shared_ptr<REP> getRep(const std::string& uid, MAP* specificMap);


  static RepManager*  mTheInstance;         ///< the only instance of this class

  VolumetricRepMap mVolumetricRepByImageMap; ///< used for caching reps based on image content
//  ImageMapperMonitorMap mImageMapperMonitorMap; ///< used for adding clip decorations to the mappers

  const int           MAX_VOLUMETRICREPS; ///< number of Volumetric reps in the pool
  std::string         mVolumetricRepNames[2]; ///< the name of the reps in the pool
  VolumetricRepMap    mVolumetricRepMap;  ///< the reps in the pool

  const int           MAX_PROGRESSIVEVOLUMETRICREPS; ///< number of ProgressiveLODVolumetric reps in the pool
  std::string         mProgressiveVolumetricRepNames[2]; ///< the name of the reps in the pool
  ProgressiveVolumetricRepMap    mProgressiveVolumetricRepMap;  ///< the reps in the pool

  const int           MAX_PROBEREPS; ///< number of Probe reps in the pool
  std::string         mProbeRepNames[2]; ///< the name of the reps in the pool
  ProbeRepMap         mProbeRepMap;  ///< the reps in the pool

  const int           MAX_LANDMARKREPS; ///< number of Landmark reps in the pool
  std::string         mLandmarkRepNames[2]; ///< the name of the reps in the pool
  LandmarkRepMap      mLandmarkRepMap;    ///< the reps in the pool

  const int           MAX_TOOLREP3DS; ///< number of Tool3D reps in the pool
  std::string         mToolRep3DNames[5]; ///< the name of the reps in the pool
  ToolRep3DMap        mToolRep3DMap;      ///< the reps in the pool

  const int           MAX_GEOMETRICREPS; ///< number of Geometric reps in the pool
  std::string         mGeometricRepNames[6]; ///< the name of the reps in the pool
  GeometricRepMap     mGeometricRepMap;      ///< the reps in the pool

  RepMap              mRepMap; ///< contains all the reps in the specific maps above. Use for simplified access.

  ssc::ToolPtr mConnectedTool; ///< the tool we are listening to transforms from (to update inria2drep points)

private:
  RepManager(); ///< creates a pool of reps
  ~RepManager(); ///< empty
  RepManager(RepManager const&); ///< not implemented
  RepManager& operator=(RepManager const&); ///< not implemented
};
RepManager* repManager();
}//namespace cx
#endif /* CXREPMANAGER_H_ */
