#ifndef CXREPMANAGER_H_
#define CXREPMANAGER_H_

#include <QObject>
#include <map>
#include <vector>
#include "sscForwardDeclarations.h"


namespace ssc
{
  typedef boost::shared_ptr<class Rep> RepPtr;
  typedef boost::shared_ptr<class ProgressiveLODVolumetricRep> ProgressiveLODVolumetricRepPtr;
}

namespace cx
{
typedef boost::shared_ptr<class LandmarkRep> LandmarkRepPtr;
//typedef ssc::ProbeRep ProbeRep;
//typedef ssc::Transform3D Transform3D;
//typedef ssc::ProbeRepPtr ProbeRepPtr;
//typedef ssc::ProgressiveLODVolumetricRep ProgressiveVolumetricRep;
//typedef ssc::ProgressiveLODVolumetricRepPtr ProgressiveVolumetricRepPtr;

typedef std::map<QString, ssc::RepPtr> RepMap;
typedef std::map<QString, ssc::VolumetricRepPtr> VolumetricRepMap;
typedef std::map<QString, ssc::ProbeRepPtr> ProbeRepMap;
typedef std::map<QString, ssc::ProgressiveLODVolumetricRepPtr> ProgressiveVolumetricRepMap;
typedef std::map<QString, LandmarkRepPtr> LandmarkRepMap;
typedef std::map<QString, ssc::ToolRep3DPtr> ToolRep3DMap;
typedef std::map<QString, ssc::GeometricRepPtr> GeometricRepMap;

//typedef std::map<QString, ssc::ImageMapperMonitorPtr> ImageMapperMonitorMap;

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

  std::vector<std::pair<QString, QString> > getRepUidsAndNames(); ///< get unique id and name of all reps in the pool

  RepMap* getReps(); ///< get all reps in the pool
  VolumetricRepMap* getVolumetricReps(); ///< get all Volumetric reps in the pool
  ProbeRepMap* getProbeReps(); ///< get all Probe reps in the pool
  ProgressiveVolumetricRepMap* getProgressiveVolumetricReps(); ///< get all ProgressiveLODVolumetric reps in the pool
//  LandmarkRepMap* getLandmarkReps(); ///< get all Landmark reps in the pool
  ToolRep3DMap* getToolRep3DReps(); ///< get all Tool3D reps in the pool
  GeometricRepMap* getGeometricReps(); ///< get all Geometric reps in the pool

  ssc::RepPtr getRep(const QString& uid); ///< get one specific rep
  ssc::VolumetricRepPtr getVolumetricRep(const QString& uid); ///< get one specific Volumetric rep
  ssc::ProbeRepPtr getProbeRep(const QString& uid); ///< get one specific Probe rep
  ssc::ProgressiveLODVolumetricRepPtr getProgressiveVolumetricRep(const QString& uid); ///< get one specific ProgressiveLODVolumetric rep
//  LandmarkRepPtr getLandmarkRep(const QString& uid); ///<  get one specific Landmark rep
  ssc::ToolRep3DPtr getToolRep3DRep(const QString& uid); ///<  get one specific Tool3D rep
  ssc::GeometricRepPtr getGeometricRep(const QString& uid); ///<  get one specific Geometric rep

//  void addToolRep3D(ssc::ToolRep3DPtr rep);
  ssc::ToolRep3DPtr getDynamicToolRep3DRep(QString uid);

  /**Get a volumetric rep based on which image you want to  display.
   * This is useful when creating the rep is expensive and should be done only once.
   */
  ssc::VolumetricRepPtr getVolumetricRep(ssc::ImagePtr image);

protected slots:
  void probeRepPointPickedSlot(double x,double y,double z);
  void dominantToolChangedSlot(const QString& toolUid); ///< makes sure the inriareps are connected to the right tool

protected:
  template<class REP, class MAP>
  boost::shared_ptr<REP> addRep(const QString& uid, MAP* specificMap);
  template<class REP, class MAP>
  boost::shared_ptr<REP> addRep(REP* rep, MAP* specificMap);
  template<class REP, class MAP>
  boost::shared_ptr<REP> addRep(boost::shared_ptr<REP> rep, MAP* specificMap);
  template<class REP, class MAP>
  boost::shared_ptr<REP> getRep(const QString& uid, MAP* specificMap);


  static RepManager*  mTheInstance;         ///< the only instance of this class

  VolumetricRepMap mVolumetricRepByImageMap; ///< used for caching reps based on image content
//  ImageMapperMonitorMap mImageMapperMonitorMap; ///< used for adding clip decorations to the mappers

  const int           MAX_VOLUMETRICREPS; ///< number of Volumetric reps in the pool
  QString         mVolumetricRepNames[2]; ///< the name of the reps in the pool
  VolumetricRepMap    mVolumetricRepMap;  ///< the reps in the pool

  const int           MAX_PROGRESSIVEVOLUMETRICREPS; ///< number of ProgressiveLODVolumetric reps in the pool
  QString         mProgressiveVolumetricRepNames[2]; ///< the name of the reps in the pool
  ProgressiveVolumetricRepMap    mProgressiveVolumetricRepMap;  ///< the reps in the pool

  const int           MAX_PROBEREPS; ///< number of Probe reps in the pool
  QString         mProbeRepNames[2]; ///< the name of the reps in the pool
  ProbeRepMap         mProbeRepMap;  ///< the reps in the pool

  const int           MAX_LANDMARKREPS; ///< number of Landmark reps in the pool
  QString         mLandmarkRepNames[2]; ///< the name of the reps in the pool
  LandmarkRepMap      mLandmarkRepMap;    ///< the reps in the pool

  const int           MAX_TOOLREP3DS; ///< number of Tool3D reps in the pool
  QString         mToolRep3DNames[5]; ///< the name of the reps in the pool
  ToolRep3DMap        mToolRep3DMap;      ///< the reps in the pool

  const int           MAX_GEOMETRICREPS; ///< number of Geometric reps in the pool
  QString         mGeometricRepNames[6]; ///< the name of the reps in the pool
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
