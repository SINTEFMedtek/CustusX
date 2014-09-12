/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXREGISTRATIONMANAGER_H_
#define CXREGISTRATIONMANAGER_H_

#include <map>
#include <vector>
#include <QObject>
#include <QDateTime>
#include "vtkSmartPointer.h"
#include "vtkForwardDeclarations.h"
#include "cxImage.h"
#include "cxRegistrationTransform.h"
#include "cxAcquisitionData.h"
#include "cxLandmark.h"
class ctkPluginContext;

namespace cx
{
class Reporter;
typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;
typedef boost::shared_ptr<class RegistrationService> RegistrationServicePtr;

/**
 * \file
 * \addtogroup cx_module_registration
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
	RegistrationManager(AcquisitionDataPtr acquisitionData, ctkPluginContext* pluginContext); ///< use getInstance instead
  virtual ~RegistrationManager() {} ///< destructor

  typedef std::pair<QString, bool> StringBoolPair; ///< name and if the point is active or not
  typedef std::map<int, StringBoolPair> NameListType; ///< landmarkindex, name and if point is active or not

  //TODO: Remove
  void restart();

  //TODO: make these 4 private
  DataPtr getFixedData();
  DataPtr getMovingData();
  void setFixedData(DataPtr fixedData);
  void setMovingData(DataPtr movingData);

  void doPatientRegistration(); ///< registrates the fixed image to the patient
  void doImageRegistration(bool translationOnly); ///< registrates the image to the fixed image
  void doFastRegistration_Orientation(const Transform3D& tMtm); ///< use the current dominant tool orientation to find patient orientation
  void doFastRegistration_Translation(); ///< use the landmarks in master image and patient to perform a translation-only landmark registration
  void applyPatientOrientation(const Transform3D& tMtm);

  AcquisitionDataPtr getAcquisitionData() { return mAcquisitionData; }

  void applyImage2ImageRegistration(Transform3D delta_pre_rMd, QString description);
  void applyPatientRegistration(Transform3D rMpr_new, QString description);

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

	AcquisitionDataPtr mAcquisitionData;

private:
  RegistrationManager(RegistrationManager const&); ///< not implemented
  RegistrationManager& operator=(RegistrationManager const&); ///< not implemented

	RegistrationServicePtr mRegistrationService;
};


/**
 * @}
 */
}//namespace cx

#endif /* CXREGISTRATIONMANAGER_H_ */
