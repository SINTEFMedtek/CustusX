#ifndef CXPORTSERVICEPROXY_H
#define CXPORTSERVICEPROXY_H

#include "cxResourceExport.h"
#include "cxPortService.h"
class ctkPluginContext;

namespace cx
{

class cxResource_EXPORT FileReaderWriterServiceProxy : public FileReaderWriterService
{
	Q_OBJECT
public:
	static FileReaderWriterServicePtr create(ctkPluginContext *context);

	FileReaderWriterServiceProxy(ctkPluginContext *context);
	virtual ~FileReaderWriterServiceProxy() {}

	virtual bool isNull();

	bool canLoad(const QString &type, const QString &filename);
	DataPtr load(const QString &uid, const QString &filename);
	QString canLoadDataType() const;
	bool readInto(DataPtr data, QString path);
	void save(DataPtr data, const QString &filename);

	ctkPluginContext *mPluginContext;
	FileReaderWriterServicePtr mService;

};
}

#endif // CXPORTSERVICEPROXY_H