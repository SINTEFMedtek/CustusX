#include "cxPortServiceProxy.h"
#include "boost/bind.hpp"
#include "cxNullDeleter.h"

namespace cx {

PortServicePtr PortServiceProxy::create(ctkPluginContext *context)
{
	return PortServicePtr(new PortServiceProxy(context));
}

PortServiceProxy::PortServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mService(PortService::getNullObject())
{
	this->initServiceListener();
}

bool PortServiceProxy::isNull()
{
	return false;
}

bool PortServiceProxy::canLoad(const QString &type, const QString &filename)
{
	return mService->canLoad(type, filename);
}

DataPtr PortServiceProxy::load(const QString &uid, const QString &filename)
{
	return mService->load(uid, filename);
}

QString PortServiceProxy::canLoadDataType() const
{
	return mService->canLoadDataType();
}

bool PortServiceProxy::readInto(DataPtr data, QString path)
{
	return mService->readInto(data, path);
}

void PortServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<PortService>(
								 mPluginContext,
								 boost::bind(&PortServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (PortService*)>(),
								 boost::bind(&PortServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}

void PortServiceProxy::onServiceAdded(PortService *service)
{
	std::cout << "Port Service added: " << service->objectName().toStdString() << std::endl;
	mService.reset(service, null_deleter());
	mService->addPort(service);
}

void PortServiceProxy::onServiceRemoved(PortService *service)
{
	std::cout << "Port Service removed: " << service->objectName().toStdString() << std::endl;
	mService = PortService::getNullObject();
	mService->removePort(service);
}

}
