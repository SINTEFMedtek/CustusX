#ifndef CXREGISTRATIONMANAGER_H_
#define CXREGISTRATIONMANAGER_H_

#include <map>
#include <vector>
#include <QObject>
#include <QDateTime>
#include "vtkSmartPointer.h"
#include "vtkForwardDeclarations.h"
#include "sscImage.h"
#include "sscRegistrationTransform.h"

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
  typedef std::pair<QString, bool> StringBoolPair; ///< name and if the point is active or not
  typedef std::map<int, StringBoolPair> NameListType; ///< landmarkindex, name and if point is active or not

  static RegistrationManager* getInstance(); ///< get the only instance of this class
  static void shutdown();
  void initialize();

  void setFixedData(ssc::DataPtr fixedData);
  ssc::DataPtr getFixedData();

  void setMovingData(ssc::DataPtr movingData);
  ssc::DataPtr getMovingData();

  void setManualPatientRegistration(ssc::Transform3D patientRegistration); ///< used for when a user wants to ???
  ssc::Transform3DPtr getManualPatientRegistration(); ///< get the manually set patient registration

  ssc::Transform3D getManualPatientRegistrationOffset(); ///< get the offset transform that moves the patient registration

  void doPatientRegistration(); ///< registrates the fixed image to the patient
  void doImageRegistration(ssc::ImagePtr image); ///< registrates the image to the fixed image
  void doFastRegistration_Orientation(const ssc::Transform3D& tMtm); ///< use the current dominant tool orientation to find patient orientation
  void doFastRegistration_Translation(); ///< use the landmarks in master image and patient to perform a translation-only landmark registration
  void doVesselRegistration(int lts_ratio = 80, double stop_delta = 0.001, double lambda = 0, double sigma = 1.0, bool lin_flag = 1, int sample = 1, int single_point_thre = 1, bool verbose = 1);

  //Interface for saving/loading
  void addXml(QDomNode& parentNode); ///< adds xml information about the registrationmanger and its variabels
  void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of the RegistrationManager.
  void clear();

public slots:
  void setManualPatientRegistrationOffsetSlot(ssc::Transform3D offset); ///< transform for (slightly) moving a patient registration

signals:
  void patientRegistrationPerformed();
  void fixedDataChanged(QString uid);
  void movingDataChanged(QString uid);

protected:
  RegistrationManager(); ///< use getInstance instead
  ~RegistrationManager(); ///< destructor

  ssc::Transform3D performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok) const;
  vtkPointsPtr convertTovtkPoints(const std::vector<QString>& uids, const ssc::LandmarkMap& data, ssc::Transform3D M);
  std::vector<ssc::Vector3D> convertAndTransformToPoints(const std::vector<QString>& uids, const ssc::LandmarkMap& data, ssc::Transform3D M);
  std::vector<QString> getUsableLandmarks(const ssc::LandmarkMap& data_a, const ssc::LandmarkMap& data_b);
  void updateRegistration(QDateTime oldTime, ssc::RegistrationTransform deltaTransform, ssc::DataPtr data, QString masterFrame);

  static RegistrationManager* mCxInstance; ///< the only instance of this class

  ssc::DataPtr mFixedData; ///< the data that shouldn't update its matrices during a registrations
  ssc::DataPtr mMovingData; ///< the data that should update its matrices during a registration

  QDateTime mLastRegistrationTime; ///< last timestamp for registration during this session. All registrations in one session results in only one reg transform.

  ssc::Transform3D mPatientRegistrationOffset; ///< manually set offset for that will be added to the patientregistration

private:
  RegistrationManager(RegistrationManager const&); ///< not implemented
  RegistrationManager& operator=(RegistrationManager const&); ///< not implemented
};

/**Shortcut for accessing the registrationmanager instance.
 */
RegistrationManager* registrationManager();

}//namespace cx

#endif /* CXREGISTRATIONMANAGER_H_ */
