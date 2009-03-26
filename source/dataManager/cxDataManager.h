#ifndef CXDATAMANAGER_H_
#define CXDATAMANAGER_H_

#include "sscDataManagerImpl.h"

namespace cx
{
/**
 * \class cxDataManager
 *
 * \brief
 *
 * \date Mar 23, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */
class DataManager : public ssc::DataManagerImpl
{
  Q_OBJECT
public:
  static DataManager* getInstance();

protected:
  DataManager(); ///< use getInstance instead
  ~DataManager(); ///< destructor

  static DataManager* mCxInstance;

private:
  DataManager(DataManager const&);
  DataManager& operator=(DataManager const&);
};
}//namespace cx
#endif /* CXDATAMANAGER_H_ */
