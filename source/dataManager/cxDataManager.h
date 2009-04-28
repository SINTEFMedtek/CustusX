#ifndef CXDATAMANAGER_H_
#define CXDATAMANAGER_H_

#include "sscDataManagerImpl.h"

class QDomNode;
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
  
public slots:
  void save(QString filename); ///< saves the application data for the active patient to XML document
  void load(QString filename); ///< loads the application data for the active patient from XML document

protected:
  DataManager(); ///< DataManager is a Singleton. Use getInstance instead
  ~DataManager(); ///< destructor
  
  QDomNode getXml(QDomDocument& doc);///< Get a XML representation of the DataManager \return A XML data representation for this object. \param doc The root of the document tree.
  void parseXml(QDomNode& datamangerNode);///< Use a XML node to load data. \param datamangerNode A XML data representation of the DataManager.

  static DataManager* mCxInstance;

private:
  DataManager(DataManager const&);
  DataManager& operator=(DataManager const&);
};
}//namespace cx
#endif /* CXDATAMANAGER_H_ */
