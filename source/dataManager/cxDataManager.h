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
  
  //Interface for saving/loading
  void addXml(QDomNode& parentNode);
  void parseXml(QDomNode& node);

signals:
  void currentImageDeleted(); ///< emitted when data is deleted
  
public slots:
  void deleteImageSlot(ssc::ImagePtr image); ///< Deletes image and emits dataDeleted signal
  
protected:
  DataManager(); ///< DataManager is a Singleton. Use getInstance instead
  ~DataManager(); ///< destructor
  
  static DataManager* mCxInstance;

private:
  DataManager(DataManager const&);
  DataManager& operator=(DataManager const&);
};
}//namespace cx
#endif /* CXDATAMANAGER_H_ */
