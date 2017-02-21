#ifndef CXTEST_RECEIVER_H
#define CXTEST_RECEIVER_H

#include "cxtest_org_custusx_core_openigtlink3_export.h"

#include "catch.hpp"

#include <QObject>
#include "ctkVTKObject.h"
#include "igtlioDevice.h"
#include "igtlioLogic.h"

#include "cxImage.h"
#include "cxTransform3D.h"

#include "cxNetworkHandler.h"

namespace cxtest
{

class CXTEST_ORG_CUSTUSX_CORE_OPENIGTLINK3_EXPORT Receiver : public QObject
{
	Q_OBJECT
	QVTK_OBJECT

public:
	Receiver(igtlio::LogicPointer logic);
	virtual ~Receiver();

	void connect(std::string ip="localhost", int port=-1);
	void listen(igtlio::DevicePointer device, bool verbose=true);

	void sendCommand();

	int number_of_events_received;
	bool image_received;
	bool transform_received;
	bool command_received;

signals:
	void done();

public slots:
	void onDeviceModifiedPrint(vtkObject *caller, void *device, unsigned long event, void *);
	void onDeviceModifiedCount(vtkObject *caller, void *device, unsigned long event, void *);

private slots:
	void checkImage(cx::ImagePtr image);
	void checkTransform(QString devicename, cx::Transform3D transform, double timestamp);
	void checkCommand(QString devicename, QString xml);

private:
	cx::NetworkHandler* mNetwork;
	igtlio::SessionPointer mSession;
};

}//namespace cxtest

#endif //CXTEST_RECEIVER_H
