#ifndef CXREGISTRATIONMANAGER_H_
#define CXREGISTRATIONMANAGER_H_

#include <map.h>
#include <QObject>
#include "vtkSmartPointer.h"
#include "sscImage.h"
/**
 * cxRegistrationManager.h
 *
 * \brief This class manages (image- and patient-) registration specific details.
 *
 * \date Feb 4, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */

typedef vtkSmartPointer<class vtkDoubleArray> vtkDoubleArrayPtr;

namespace cx
{
class RegistrationManager : public QObject
{
  Q_OBJECT

public:
  typedef std::pair<std::string, bool> StringBoolPair;
  typedef std::map<int, StringBoolPair> NameListType;

  static RegistrationManager* getInstance();

  void setMasterImage(ssc::ImagePtr image);
  ssc::ImagePtr getMasterImage();
  bool isMasterImageSet();

  void setGlobalPointSet(vtkDoubleArrayPtr pointset);
  vtkDoubleArrayPtr getGlobalPointSet();

  void setGlobalPointSetNameList(NameListType nameList);
  NameListType getGlobalPointSetNameList();

public slots:
  void setGlobalPointsNameSlot(int index, std::string name); ///< set the points (user) name
  void setGlobalPointsActiveSlot(int index, bool active); ///< set if the point should be used in matrix calc or not

protected:
  RegistrationManager(); ///< use getInstance instead
  ~RegistrationManager(); ///< destructor

  static RegistrationManager* mCxInstance;

  ssc::ImagePtr mMasterImage;
  vtkDoubleArrayPtr mGlobalPointSet;
  NameListType mGlobalPointSetNameList;

private:
  RegistrationManager(RegistrationManager const&);
  RegistrationManager& operator=(RegistrationManager const&);
};
}//namespace cx

#endif /* CXREGISTRATIONMANAGER_H_ */
