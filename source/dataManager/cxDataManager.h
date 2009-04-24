#ifndef CXDATAMANAGER_H_
#define CXDATAMANAGER_H_

#include "sscDataManagerImpl.h"
#include <QDomNode>

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
  
public slots:
  void save(QString filename); ///< saves the application data for the active patient to XML document
  void load(QString filename); ///< loads the application data for the active patient from XML document

protected:
  DataManager(); ///< use getInstance instead
  ~DataManager(); ///< destructor
  
  QDomNode GetXml(); ///< return the XML data representation for the Datamanger
  void ParseXml(QDomNode* datamangerNode); ///< input the XML datarepresentation for the DataManager

  static DataManager* mCxInstance;

private:
  DataManager(DataManager const&);
  DataManager& operator=(DataManager const&);
};
}//namespace cx
#endif /* CXDATAMANAGER_H_ */
