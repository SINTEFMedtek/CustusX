#include "cxPortServiceProxy.h"
#include "boost/bind.hpp"
#include "cxNullDeleter.h"

namespace cx {

FileReaderWriterServicePtr FileReaderWriterServiceProxy::create(ctkPluginContext *context)
{
	return FileReaderWriterServicePtr(new FileReaderWriterServiceProxy(context));
}

FileReaderWriterServiceProxy::FileReaderWriterServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mService(FileReaderWriterService::getNullObject())
{
	//this->initServiceListener();
}

bool FileReaderWriterServiceProxy::isNull()
{
	return mService->isNull();
}

bool FileReaderWriterServiceProxy::canLoad(const QString &type, const QString &filename)
{
	return mService->canLoad(type, filename);
}

DataPtr FileReaderWriterServiceProxy::load(const QString &uid, const QString &filename)
{
	return mService->load(uid, filename);
}

QString FileReaderWriterServiceProxy::canLoadDataType() const
{
	return mService->canLoadDataType();
}

bool FileReaderWriterServiceProxy::readInto(DataPtr data, QString path)
{
	return mService->readInto(data, path);
}
/*

void FileReaderWriterServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<FileReaderWriterService>(
								 mPluginContext,
								 boost::bind(&FileReaderWriterServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (FileReaderWriterService*)>(),
								 boost::bind(&FileReaderWriterServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}

void FileReaderWriterServiceProxy::onServiceAdded(FileReaderWriterService *service)
{
	std::cout << "Port Service added: " << service->objectName().toStdString() << std::endl;
	//mService.reset(service, null_deleter());
	//mService->addPort(service);
}

void FileReaderWriterServiceProxy::onServiceRemoved(FileReaderWriterService *service)
{
	std::cout << "Port Service removed: " << service->objectName().toStdString() << std::endl;
	//mService = FileReaderWriterService::getNullObject();
	//mService->removePort(service);
}
*/

}
