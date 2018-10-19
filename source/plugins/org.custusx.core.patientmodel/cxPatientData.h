/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
