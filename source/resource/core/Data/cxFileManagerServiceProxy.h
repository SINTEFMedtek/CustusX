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
	vtkImageDataPtr loadVtkImageData(QString filename);
	vtkPolyDataPtr loadVtkPolyData(QString filename);

	void save(DataPtr data, const QString &filename);
	void addPort(FileReaderWriterService *service);
	void removePort(FileReaderWriterService *service);

private:
	void initServiceListener();
	void onServiceAdded(FileManagerService *service);
	void onServiceRemoved(FileManagerService *service);

	boost::shared_ptr<ServiceTrackerListener<FileManagerService> > mServiceListener;

	ctkPluginContext *mPluginContext;
	FileManagerServicePtr mService;

};
}

#endif // CXFILEMANAGERSERVICEPROXY_H