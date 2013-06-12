// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
#include "cxAcquisitionData.h"

namespace cx
{
class MessageManager;
typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;

/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

/**\
 * \class RegistrationManager
 *
 * \brief This class manages (image- and patient-) registration specific details.
 *
 * \date Feb 4, 2009
 * \\author Janne Beate Bakeng, SINTEF
 */
class RegistrationManager : public QObject
{
  Q_OBJECT

public:
  RegistrationManager(AcquisitionDataPtr acquisitionData); ///< use getInstance instead
  virtual ~RegistrationManager() {} ///< destructor

  typedef std::pair<QString, bool> StringBoolPair; ///< name and if the point is active or not
  typedef std::map<int, StringBoolPair> NameListType; ///< landmarkindex, name and if point is active or not

  void restart();

  void setFixedData(ssc::DataPtr fixedData);
  ssc::DataPtr getFixedData();

  void setMovingData(ssc::DataPtr movingData);
  ssc::DataPtr getMovingData();

  void doPatientRegistration(); ///< registrates the fixed image to the patient
  void doImageRegistration(bool translationOnly); ///< registrates the image to the fixed image
  void doFastRegistration_Orientation(const ssc::Transform3D& tMtm); ///< use the current dominant tool orientation to find patient orientation
  void doFastRegistration_Translation(); ///< use the landmarks in master image and patient to perform a translation-only landmark registration
  void applyPatientOrientation(const ssc::Transform3D& tMtm);

  AcquisitionDataPtr getAcquisitionData() { return mAcquisitionData; }

  void applyImage2ImageRegistration(ssc::Transform3D delta_pre_rMd, QString description);
  void applyPatientRegistration(ssc::Transform3D rMpr_new, QString description);

private slots:
  void clearSlot();
	void duringSavePatientSlot();
	void duringLoadPatientSlot();

signals:
  void fixedDataChanged(QString uid);
  void movingDataChanged(QString uid);

protected:
  //Interface for saving/loading
  void addXml(QDomNode& parentNode); ///< adds xml information about the registrationmanger and its variabels
  void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of the RegistrationManager.

  ssc::Transform3D performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok) const;
  vtkPointsPtr convertTovtkPoints(const std::vector<QString>& uids, const ssc::LandmarkMap& data, ssc::Transform3D M);
  std::vector<ssc::Vector3D> convertAndTransformToPoints(const std::vector<QString>& uids, const ssc::LandmarkMap& data, ssc::Transform3D M);
  std::vector<ssc::Vector3D> convertVtkPointsToPoints(vtkPointsPtr base);
  std::vector<QString> getUsableLandmarks(const ssc::LandmarkMap& data_a, const ssc::LandmarkMap& data_b);
  void updateRegistration(QDateTime oldTime, ssc::RegistrationTransform deltaTransform, ssc::DataPtr data, QString masterFrame);
  void writePreLandmarkRegistration(QString name, ssc::LandmarkMap landmarks);

  ssc::DataPtr mFixedData; ///< the data that shouldn't update its matrices during a registrations
  ssc::DataPtr mMovingData; ///< the data that should update its matrices during a registration

  QDateTime mLastRegistrationTime; ///< last timestamp for registration during this session. All registrations in one session results in only one reg transform.

	AcquisitionDataPtr mAcquisitionData;

private:
  RegistrationManager(RegistrationManager const&); ///< not implemented
  RegistrationManager& operator=(RegistrationManager const&); ///< not implemented
};

/**Shortcut for accessing the registrationmanager instance.
 */
//RegistrationManager* registrationManager();

/**
 * @}
 */
}//namespace cx

#endif /* CXREGISTRATIONMANAGER_H_ */
