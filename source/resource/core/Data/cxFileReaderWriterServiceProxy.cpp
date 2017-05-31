#include "cxFileReaderWriterServiceProxy.h"
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

void FileReaderWriterServiceProxy::save(DataPtr data, const QString &filename)
{
	mService->save(data, filename);
}


}
