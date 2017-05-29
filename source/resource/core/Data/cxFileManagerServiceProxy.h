#ifndef CXFILEMANAGERSERVICEPROXY_H
#define CXFILEMANAGERSERVICEPROXY_H

#include "cxResourceExport.h"
#include "cxFileManagerService.h"
#include "cxServiceTrackerListener.h"

namespace cx
{

class cxResource_EXPORT FileManagerServiceProxy : public FileManagerService
{
	Q_OBJECT
public:
	static FileManagerServicePtr create(ctkPluginContext *context);

	FileManagerServiceProxy(ctkPluginContext *context);
	virtual ~FileManagerServiceProxy();

	virtual bool isNull();

	bool canLoad(const QString &type, const QString &filename);
	DataPtr load(const QString &uid, const QString &filename);
	QString canLoadDataType() const;
	bool readInto(DataPtr data, QString path);
	QString findDataTypeFromFile(QString filename);

	/*
private:
	void initServiceListener();
	void onServiceAdded(FileReaderWriterService *service);
	void onServiceRemoved(FileReaderWriterService *service);

	boost::shared_ptr<ServiceTrackerListener<FileReaderWriterService> > mServiceListener;
*/
	ctkPluginContext *mPluginContext;
	FileManagerServicePtr mService;

};
}

#endif // CXFILEMANAGERSERVICEPROXY_H