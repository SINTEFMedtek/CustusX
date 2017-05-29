#include "cxFileManagerServiceProxy.h"

namespace cx
{

FileManagerServicePtr FileManagerServiceProxy::create(ctkPluginContext *context)
{
	return FileManagerServicePtr(new FileManagerServiceProxy(context));
}

FileManagerServiceProxy::FileManagerServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mService(FileManagerService::getNullObject())
{

}

FileManagerServiceProxy::~FileManagerServiceProxy()
{}

bool FileManagerServiceProxy::isNull()
{
	return mService->isNull();
}

bool FileManagerServiceProxy::canLoad(const QString &type, const QString &filename)
{
	return mService->canLoad(type, filename);
}

DataPtr FileManagerServiceProxy::load(const QString &uid, const QString &filename)
{
	return mService->load(uid, filename);
}

QString FileManagerServiceProxy::canLoadDataType() const
{
	return mService->canLoadDataType();
}

bool FileManagerServiceProxy::readInto(DataPtr data, QString path)
{
	return mService->readInto(data, path);
}

QString FileManagerServiceProxy::findDataTypeFromFile(QString filename)
{
	return mService->findDataTypeFromFile(filename);
}


}
