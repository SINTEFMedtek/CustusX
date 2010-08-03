#ifndef CXDATAMANAGER_H_
#define CXDATAMANAGER_H_

#include <sscDataManagerImpl.h>
#include <QDomNode>

class QDomDocument;

namespace cx
{
/**
 * \class cxDataManager
 *
 * \brief cx implementation of additional functionality for the ssc::DataManager
 *
 * \date Mar 23, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class DataManager : public ssc::DataManagerImpl
{
  Q_OBJECT
public:
  static DataManager* getInstance();
  virtual ~DataManager();
  
  static void initialize();
  bool getDebugMode() const;

signals:
  void currentImageDeleted(ssc::ImagePtr image); ///< emitted when data is deleted
  void debugModeChanged(bool on);
public slots:
  void deleteImageSlot(ssc::ImagePtr image); ///< Deletes image and emits dataDeleted signal
  void setDebugMode(bool on);
  
protected:
  DataManager(); ///< DataManager is a Singleton. Use getInstance instead
  
  //static DataManager* mCxInstance;

  bool mDebugMode; ///< if set: allow lots of weird debug stuff.

private:
  DataManager(DataManager const&);
  DataManager& operator=(DataManager const&);
};
//DataManager* dataManager();
}//namespace cx
#endif /* CXDATAMANAGER_H_ */
