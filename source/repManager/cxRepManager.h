#ifndef CXREPMANAGER_H_
#define CXREPMANAGER_H_

#include <QObject>
#include <map>
#include <vector>
#include "sscForwardDeclarations.h"
#include "sscMessageManager.h"
#include "sscTool.h"
#include "sscTypeConversions.h"


namespace ssc
{
  typedef boost::shared_ptr<class Rep> RepPtr;
  typedef boost::shared_ptr<class ProgressiveLODVolumetricRep> ProgressiveLODVolumetricRepPtr;
}

namespace cx
{

typedef std::map<QString, ssc::RepPtr> RepMap;
typedef std::map<QString, ssc::VolumetricRepPtr> VolumetricRepMap;

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

  template<class REP>
  boost::shared_ptr<REP> findFirstRep(std::vector<ssc::RepPtr> reps, ssc::ToolPtr tool)
  {
    for(unsigned i=0; i<reps.size() ; ++i)
    {
      boost::shared_ptr<REP> rep = boost::shared_dynamic_cast<REP>(reps[i]);
      if(rep && rep->hasTool(tool))
      {
        return rep;
      }
    }
    return boost::shared_ptr<REP>();
  }

  template<class REP>
  boost::shared_ptr<REP> findFirstRep(std::vector<ssc::RepPtr> reps, ssc::DataPtr data)
  {
    for(unsigned i=0; i<reps.size() ; ++i)
    {
      boost::shared_ptr<REP> rep = boost::shared_dynamic_cast<REP>(reps[i]);
      if(rep && rep->hasData(data))
        return rep;
    }
    return boost::shared_ptr<REP>();
  }

  template<class REP>
  boost::shared_ptr<REP> findFirstRep(std::vector<ssc::RepPtr> reps)
  {
    for(unsigned i=0; i<reps.size() ; ++i)
    {
      boost::shared_ptr<REP> rep = boost::shared_dynamic_cast<REP>(reps[i]);
      if(rep)
        return rep;
    }
    return boost::shared_ptr<REP>();
  }

  /**Get a volumetric rep based on which image you want to  display.
   * This is useful when creating the rep is expensive and should be done only once.
   */
  ssc::VolumetricRepPtr getVolumetricRep(ssc::ImagePtr image);

protected slots:
  void volumeRemovedSlot(QString uid);

protected:
//  template<class REP, class MAP>
//  boost::shared_ptr<REP> addRep(const QString& uid, MAP* specificMap);
//  template<class REP, class MAP>
//  boost::shared_ptr<REP> addRep(REP* rep, MAP* specificMap);
//  template<class REP, class MAP>
//  boost::shared_ptr<REP> addRep(boost::shared_ptr<REP> rep, MAP* specificMap);
//  template<class REP, class MAP>
//  boost::shared_ptr<REP> getRep(const QString& uid, MAP* specificMap);


  static RepManager*  mTheInstance;         ///< the only instance of this class

  VolumetricRepMap mVolumetricRepByImageMap; ///< used for caching reps based on image content

  RepMap              mRepMap; ///< contains all the reps in the specific maps above. Use for simplified access.

private:
  RepManager(); ///< creates a pool of reps
  ~RepManager(); ///< empty
  RepManager(RepManager const&); ///< not implemented
  RepManager& operator=(RepManager const&); ///< not implemented


};
RepManager* repManager();
}//namespace cx
#endif /* CXREPMANAGER_H_ */
