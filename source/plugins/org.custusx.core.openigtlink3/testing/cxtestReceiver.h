#ifndef CXTEST_RECEIVER_H
#define CXTEST_RECEIVER_H

#include "org_custusx_core_openigtlink3_Export.h"

#include "catch.hpp"

#include <QObject>
#include "ctkVTKObject.h"
#include "vtkIGTLIODevice.h"

namespace cxtest
{

class org_custusx_core_openigtlink3_EXPORT Receiver : public QObject
{
	Q_OBJECT
	QVTK_OBJECT

public:
	Receiver();
	virtual ~Receiver();

	void listen(vtkIGTLIODevicePointer device);

	int mEventsReceived;

public slots:
	void onDeviceModified(vtkObject *caller, void *device, unsigned long event, void *);

};

}//namespace cxtest

#endif //CXTEST_RECEIVER_H
