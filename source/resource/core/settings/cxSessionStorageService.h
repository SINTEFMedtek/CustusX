/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSESSIONSTORAGESERVICE_H
#define CXSESSIONSTORAGESERVICE_H

#include "cxResourceExport.h"

#include "boost/shared_ptr.hpp"
#include <QString>
#include <QObject>
#include "cxForwardDeclarations.h"

class QDomElement;

#define SessionStorageService_iid "cx::SessionStorageService"

namespace cx
{
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;

/** Persistent storage for the current session.
 *
 * The storage consist of a root folder unique for this session,
 * and an xml input/output.
 * The folder structured is available whenever session isValid().
 * The xml tree is available during load/save only. Clients are supposed
 * to listen to isSaving/isLoading, then save/load their xml data.
 *
 * \ingroup cx_resource_core_settings
 * \date 2014-12-01
 * \author Christian Askeland
 */
class cxResource_EXPORT SessionStorageService : public QObject
{
	Q_OBJECT
public:
    SessionStorageService();
    virtual ~SessionStorageService();

	virtual void load(QString dir) = 0; ///< load session from dir, or create new session in this location if none exist
	virtual void save() = 0; ///< Save all application data to XML file
	virtual void clear() = 0;
	virtual bool isValid() const = 0;
	virtual QString getRootFolder() const = 0;

	virtual bool isNull() const = 0;
	static SessionStorageServicePtr getNullObject();

	QString getSubFolder(QString relative); ///< return and create a folder/path relative to root. Created if necessary.

signals:
	void cleared(); ///< emitted when session is cleared, before isLoading is called
	void sessionChanged(); ///< emitted after change to a new session (new or loaded or cleared)
	void isSaving(QDomElement& root); ///< xml storage is available
	void isSavingSecond(QDomElement& root); ///< Triggered after the isSaving signal, to allow for data that
	void isLoading(QDomElement& root); ///< emitted while loading a session. Xml storage is available, getRootFolder() is set to loaded value.
	void isLoadingSecond(QDomElement& root); ///< Emitted after the isLoading signal, to allow for structures that must be loaded after core structures like PatientModel
};

} // namespace cx

Q_DECLARE_INTERFACE(cx::SessionStorageService, SessionStorageService_iid)

#endif // CXSESSIONSTORAGESERVICE_H
