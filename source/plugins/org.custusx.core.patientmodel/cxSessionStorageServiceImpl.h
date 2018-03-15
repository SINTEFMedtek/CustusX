/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSESSIONSTORAGESERVICEIMPL_H
#define CXSESSIONSTORAGESERVICEIMPL_H

#include "org_custusx_core_patientmodel_Export.h"

#include "cxSessionStorageService.h"
class QDomDocument;
class ctkPluginContext;

namespace cx
{


/**
 *
 * \ingroup cx_org_custusx_core_patientmodel
 * \date 2014-12-03
 * \author Christian Askeland
 */
class org_custusx_core_patientmodel_EXPORT SessionStorageServiceImpl : public SessionStorageService
{
	Q_OBJECT
public:
	SessionStorageServiceImpl(ctkPluginContext* context);
	virtual ~SessionStorageServiceImpl();
	virtual void load(QString dir); ///< load session from dir, or create new session in this location if none exist
	virtual void save(); ///< Save all application data to XML file
	virtual void clear();
	virtual bool isValid() const;
	virtual QString getRootFolder() const;
	virtual bool isNull() const;

private slots:
	void onSessionChanged();
	void startupLoadPatient();
private:
	void reportActivePatient();
	void setActivePatient(const QString& activePatientFolder); ///< set the activepatientfolder (absolute path)
	QString getXmlFileName() const;

	bool isValidSessionFolder(QString dir) const;
	bool folderExists(QString dir) const;
	void loadSession(QString dir);
	void initializeNewSession(QString dir);
	bool isActivePatient(QString patient) const;
	void loadPatientSilent(QString choosenDir);
	void writeRecentPatientData();
	void generateSaveDoc(QDomDocument& doc);
	void clearPatientSilent();
	QString getNoPatientFolder() const;
	void createPatientFolders(QString dir);
	static QString getVersionName();
	QString convertToValidFolderName(QString dir) const;
	void clearCache(); ///< Clear the global cache used by the entire application (cx::DataLocations::getCachePath()).
	QString getCommandLineStartupPatient();

	QString mActivePatientFolder; ///< Folder for storing the files for the active patient. Path relative to globalPatientDataFolder.
};

} // namespace cx

#endif // CXSESSIONSTORAGESERVICEIMPL_H
