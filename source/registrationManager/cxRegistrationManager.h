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
typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;
typedef vtkSmartPointer<class vtkLandmarkTransform> vtkLandmarkTransformPtr;

namespace cx
{
class ToolManager;

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

  void setActivePointsVector(std::vector<bool> vector);
  std::vector<bool> getActivePointsVector();

  void doPatientRegistration(); ///< registrates the master image to the patient
  void doImageRegistration(ssc::ImagePtr image); ///< registrates the image to the master image

public slots:
  void setGlobalPointsNameSlot(int index, std::string name); ///< set the points (user) name
  void setGlobalPointsActiveSlot(int index, bool active); ///< set if the point should be used in matrix calc or not

protected:
  RegistrationManager(); ///< use getInstance instead
  ~RegistrationManager(); ///< destructor

  static RegistrationManager* mCxInstance;

  ToolManager* mToolManager;

  ssc::ImagePtr mMasterImage;
  vtkDoubleArrayPtr mGlobalPointSet;
  NameListType mGlobalPointSetNameList;
  std::vector<bool> mActivePointsVector;

private:
  RegistrationManager(RegistrationManager const&);
  RegistrationManager& operator=(RegistrationManager const&);
};
}//namespace cx

#endif /* CXREGISTRATIONMANAGER_H_ */
