#ifndef CXFILEMANAGERSERVICENULL_H
#define CXFILEMANAGERSERVICENULL_H

#include "cxFileManagerService.h"

namespace cx
{

class cxResource_EXPORT FileManagerServiceNull : public FileManagerService
{
	Q_OBJECT
public:
	FileManagerServiceNull();
	virtual ~FileManagerServiceNull();

	virtual bool isNull();

	bool canLoad(const QString &type, const QString &filename);
	DataPtr load(const QString &uid, const QString &filename);
	QString canLoadDataType() const;
	bool readInto(DataPtr data, QString path);
	QString findDataTypeFromFile(QString filename);

};

}

#endif // CXFILEMANAGERSERVICENULL_H