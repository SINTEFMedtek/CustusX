/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSessionStorageService.h"
#include "cxNullDeleter.h"
#include <QDir>

namespace cx
{

SessionStorageService::SessionStorageService()
{

}

SessionStorageService::~SessionStorageService()
{

}

class SessionStorageServiceNull : public SessionStorageService
{
public:
	virtual ~SessionStorageServiceNull() {}
	virtual void load(QString dir) {}
	virtual void save() {}
	virtual void clear() {}
	virtual bool isValid() const { return false; }
	virtual QString getRootFolder() const { return ""; }

	virtual bool isNull() const { return true; }
};

SessionStorageServicePtr SessionStorageService::getNullObject()
{
	static SessionStorageServicePtr mNull;
	if (!mNull)
		mNull.reset(new SessionStorageServiceNull, null_deleter());
	return mNull;
}

QString SessionStorageService::getSubFolder(QString relative)
{
	QString root = this->getRootFolder();
	if (root.isEmpty())
		return "";

	QString path = root + "/" + relative;
	QDir dir(path);
	if (!dir.exists())
		dir.mkpath(path);
	return path;
}


} // cx
