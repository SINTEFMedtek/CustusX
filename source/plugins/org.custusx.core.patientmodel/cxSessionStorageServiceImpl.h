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

#ifndef CXSESSIONSTORAGESERVICEIMPL_H
#define CXSESSIONSTORAGESERVICEIMPL_H

#include "org_custusx_core_patientmodel_Export.h"

#include "cxSessionStorageService.h"
#include <QDomDocument>
class QDomDocument;
class ctkPluginContext;

namespace cx
{

///**given a root node, use the /-separated path to descend
// * into the root children recursively. Create elements if
// * necessary.
// *
// */
//cxResource_EXPORT QDomElement getElementForced(QDomNode root, QString path);

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
public:
	void generateSaveDoc(QDomDocument& doc);
private:
	void clearPatientSilent();
	QString getNoPatientFolder() const;
	void createPatientFolders(QString dir);
	static QString getVersionName();
	QDomDocument readXmlFile(QString filename);
	void writeXmlFile(QDomDocument doc, QString filename);
	QString convertToValidFolderName(QString dir) const;
	void clearCache(); ///< Clear the global cache used by the entire application (cx::DataLocations::getCachePath()).
	QString getCommandLineStartupPatient();

	QString mActivePatientFolder; ///< Folder for storing the files for the active patient. Path relative to globalPatientDataFolder.
};

} // namespace cx

#endif // CXSESSIONSTORAGESERVICEIMPL_H
