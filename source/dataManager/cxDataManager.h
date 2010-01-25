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
 * \author: Janne Beate Bakeng, SINTEF
 */
class DataManager : public ssc::DataManagerImpl
{
  Q_OBJECT
public:
  static DataManager* getInstance();
  
  //Interface for saving/loading
  void addXml(QDomNode& parentNode);
  void parseXml(QDomNode& node);
  bool write(QString& folder);
  bool load(QString& folder);

public slots:
  //TODO REMOVE
  //QDomDocument save(); ///< saves the application data for the active patient to XML document
  //void load(QDomDocument& doc); ///< loads the application data for the active patient from XML document

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
