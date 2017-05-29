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

#ifndef CXPATIENTDATA_H_
#define CXPATIENTDATA_H_

#include "org_custusx_core_patientmodel_Export.h"

#include "boost/shared_ptr.hpp"
#include <QString>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"
#include <QDomDocument>

class QDomDocument;

namespace cx
{

typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;
typedef boost::shared_ptr<class DataManager> DataServicePtr;


/**
 * \brief Functionality for storing patient data in a folder on the disk
 * and access to these data.
 * \ingroup org_custusx_core_patientmodel
 *
 * Keeps track of a single active patient (patient folder).
 * Only one such patient can be active at a time, since the
 * global managers are updated by this class.
 *
 *   \date May 18, 2010
 *   \author christiana
 *
 */
class org_custusx_core_patientmodel_EXPORT PatientData: public QObject
{
Q_OBJECT
public:
	explicit PatientData(DataServicePtr dataManager, SessionStorageServicePtr session, FileManagerServicePtr fileManager);
	virtual ~PatientData();

	QString getActivePatientFolder() const;
	bool isPatientValid() const;

public slots:
	/** \brief Import data into CustusX
	 * \param fileName The file name of the imported file(s)
	 * \param[out] infoText Information about any errors/warnings that occurred during import
	 */
	DataPtr importData(QString fileName, QString &infoText);
	void removeData(QString uid);
	void exportPatient(PATIENT_COORDINATE_SYSTEM externalSpace);
	void autoSave();

signals:
	void patientChanged();

private slots:
	void onCleared();
	void onSessionLoad(QDomElement& node);
	void onSessionSave(QDomElement& node);

private:
	DataServicePtr mDataManager;
	SessionStorageServicePtr mSession;
	FileManagerServicePtr mFileManagerService;
};

typedef boost::shared_ptr<PatientData> PatientDataPtr;

} // namespace cx

#endif /* CXPATIENTDATA_H_ */
