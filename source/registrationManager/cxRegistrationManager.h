#ifndef CXREGISTRATIONMANAGER_H_
#define CXREGISTRATIONMANAGER_H_

#include <map>
#include <QObject>
#include "vtkSmartPointer.h"
#include "sscImage.h"

typedef vtkSmartPointer<class vtkDoubleArray> vtkDoubleArrayPtr;
typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;
typedef vtkSmartPointer<class vtkLandmarkTransform> vtkLandmarkTransformPtr;

namespace cx
{
class ToolManager;
class MessageManager;

/**\
 * \class RegistrationManager
 *
 * \brief This class manages (image- and patient-) registration specific details.
 *
 * \date Feb 4, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */
class RegistrationManager : public QObject
{
  Q_OBJECT

public:
  typedef std::pair<std::string, bool> StringBoolPair; ///< name and if the point is active or not
  typedef std::map<int, StringBoolPair> NameListType; ///< landmarkindex, name and if point is active or not

  static RegistrationManager* getInstance(); ///< get the only instance of this class

  void setMasterImage(ssc::ImagePtr image); ///< set a master image used when registrating
  ssc::ImagePtr getMasterImage(); ///< get the master image
  bool isMasterImageSet(); ///< check if the master image is set

  void setGlobalPointSet(vtkDoubleArrayPtr pointset); ///< set a global point set used to register against
  vtkDoubleArrayPtr getGlobalPointSet(); ///< get the global point set

  void setGlobalPointSetNameList(NameListType nameList); ///< set user specific names on the global points
  NameListType getGlobalPointSetNameList(); ///< get a map of the names

  void doPatientRegistration(); ///< registrates the master image to the patient
  void doImageRegistration(ssc::ImagePtr image); ///< registrates the image to the master image

public slots:
  void setGlobalPointsNameSlot(int index, std::string name); ///< set the points (user) name
  void setGlobalPointsActiveSlot(int index, bool active); ///< set if the point should be used in matrix calc or not

protected:
  RegistrationManager(); ///< use getInstance instead
  ~RegistrationManager(); ///< destructor

  static RegistrationManager* mCxInstance; ///< the only instance of this class

  ToolManager* mToolManager; ///< interface to the navigation system
  MessageManager* mMessageManager; ///< takes messages intended for the user

  ssc::ImagePtr mMasterImage; ///< the master image used to register all other images against
  vtkDoubleArrayPtr mGlobalPointSet; ///< the pointset used when doing the registration
  NameListType mGlobalPointSetNameList; ///< names of the points in the global point set

private:
  RegistrationManager(RegistrationManager const&); ///< not implemented
  RegistrationManager& operator=(RegistrationManager const&); ///< not implemented
};
}//namespace cx

#endif /* CXREGISTRATIONMANAGER_H_ */
