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

#ifndef CXPATIENTMODELSERVICENULL_H
#define CXPATIENTMODELSERVICENULL_H

#include "cxResourceExport.h"
#include "cxPatientModelService.h"

namespace cx
{

/** \brief Null Object Pattern for Registration service
 *
 *
 *  \ingroup cx_resource_core_data
 *  \date 2014-09-10
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT PatientModelServiceNull : public PatientModelService
{
public:
	PatientModelServiceNull();
	virtual void insertData(DataPtr data);
	virtual DataPtr createData(QString type, QString uid, QString name="");
	virtual std::map<QString, DataPtr> getDatas(DataFilter filter) const;

	virtual LandmarksPtr getPatientLandmarks() const;
	virtual std::map<QString, LandmarkProperty> getLandmarkProperties() const;
	virtual void setLandmarkName(QString uid, QString name);
	virtual void setLandmarkActive(QString uid, bool active);

	virtual RegistrationHistoryPtr get_rMpr_History() const;

	virtual ActiveDataPtr getActiveData() const;

	virtual CLINICAL_VIEW getClinicalApplication() const;
	virtual void setClinicalApplication(CLINICAL_VIEW application);

	virtual std::map<QString, cx::VideoSourcePtr> getStreams() const;

	virtual QString getActivePatientFolder() const;
	virtual bool isPatientValid() const;
	virtual DataPtr importData(QString fileName, QString &infoText);
	virtual void exportPatient(PATIENT_COORDINATE_SYSTEM externalSpace);
	virtual void removeData(QString uid);

	virtual PresetTransferFunctions3DPtr getPresetTransferFunctions3D() const;

	virtual void setCenter(const Vector3D& center);
	virtual Vector3D getCenter() const;
    void setOperatingTable(const OperatingTable &ot);
    OperatingTable getOperatingTable() const;

	virtual QString addLandmark();

	virtual void autoSave();
	virtual bool isNull();
	virtual void makeAvailable(const QString& uid, bool available);

private:
	void printWarning() const;
};

} //cx
#endif // CXPATIENTMODELSERVICENULL_H
