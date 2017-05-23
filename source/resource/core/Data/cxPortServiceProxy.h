#ifndef CXPORTSERVICEPROXY_H
#define CXPORTSERVICEPROXY_H

#include "cxResourceExport.h"
#include "cxPortService.h"
#include "cxServiceTrackerListener.h"

namespace cx
{

class cxResource_EXPORT PortServiceProxy : public PortService
{
	Q_OBJECT
public:
	static PortServicePtr create(ctkPluginContext *context);

	PortServiceProxy(ctkPluginContext *context);
	virtual ~PortServiceProxy() {}

	virtual bool isNull();

public:
	bool canLoad(const QString &type, const QString &filename);
	DataPtr load(const QString &uid, const QString &filename);
	QString canLoadDataType() const;
	bool readInto(DataPtr data, QString path);

private:
	void initServiceListener();
	void onServiceAdded(PortService *service);
	void onServiceRemoved(PortService *service);

	ctkPluginContext *mPluginContext;
	PortServicePtr mService;
	boost::shared_ptr<ServiceTrackerListener<PortService> > mServiceListener;


};
}

#endif // CXPORTSERVICEPROXY_H