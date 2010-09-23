#ifndef CXREGISTRATIONMANAGER_H_
#define CXREGISTRATIONMANAGER_H_

#include <map>
#include <vector>
#include <QObject>
#include <QDateTime>
#include "vtkSmartPointer.h"
#include "sscImage.h"
#include "sscRegistrationTransform.h"

typedef vtkSmartPointer<class vtkDoubleArray> vtkDoubleArrayPtr;
typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;
typedef vtkSmartPointer<class vtkLandmarkTransform> vtkLandmarkTransformPtr;

namespace cx
{
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
  static void shutdown();
  void initialize();

  void setMasterImage(ssc::ImagePtr image); ///< set a master image used when registrating
  ssc::ImagePtr getMasterImage(); ///< get the master image
  bool isMasterImageSet(); ///< check if the master image is set

  void setManualPatientRegistration(ssc::Transform3D patientRegistration); ///< used for when a user wants to ???
  ssc::Transform3DPtr getManualPatientRegistration(); ///< get the manually set patient registration
//  void resetManualPatientientRegistration(); ///< tells the system not to use a manually added patient registration, after it uses landmarks for patient registration instead

  ssc::Transform3D getManualPatientRegistrationOffset(); ///< get the offset transform that moves the patient registration
//  void resetOffset(); ///< removes the offset, after it uses landmarks for patient registration instead

  void doPatientRegistration(); ///< registrates the master image to the patient
  void doImageRegistration(ssc::ImagePtr image); ///< registrates the image to the master image

  //Interface for saving/loading
  void addXml(QDomNode& parentNode); ///< adds xml information about the registrationmanger and its variabels
  void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of the RegistrationManager.
  void clear();

public slots:
  void setManualPatientRegistrationOffsetSlot(ssc::Transform3D offset); ///< transform for (slightly) moving a patient registration

signals:
  void imageRegistrationPerformed();
  void patientRegistrationPerformed();

protected:
  RegistrationManager(); ///< use getInstance instead
  ~RegistrationManager(); ///< destructor

  ssc::Transform3D performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok) const;
  vtkPointsPtr convertTovtkPoints(const std::vector<std::string>& uids, const ssc::LandmarkMap& data, ssc::Transform3D M);
  std::vector<std::string> getUsableLandmarks(const ssc::LandmarkMap& data_a, const ssc::LandmarkMap& data_b);
  void updateRegistration(QDateTime oldTime, ssc::RegistrationTransform deltaTransform, ssc::DataPtr data, QString masterFrame);

  static RegistrationManager* mCxInstance; ///< the only instance of this class

  ssc::ImagePtr mMasterImage; ///< the master image used to register all other images against
  QDateTime mLastRegistrationTime; ///< last timestamp for registration during this session. All registrations in one session results in only one reg transform.

  ssc::Transform3D mPatientRegistrationOffset; ///< manually set offset for that will be added to the patientregistration
  //ssc::Transform3D mManualPatientRegistration; ///< patient registration loaded from file

private:
  RegistrationManager(RegistrationManager const&); ///< not implemented
  RegistrationManager& operator=(RegistrationManager const&); ///< not implemented
};
/**Shortcut for accessing the registrationmanager instance.
 */
RegistrationManager* registrationManager();
}//namespace cx

#endif /* CXREGISTRATIONMANAGER_H_ */
