#ifndef CXTEST_RECEIVER_H
#define CXTEST_RECEIVER_H

#include "cxtest_org_custusx_core_openigtlink3_export.h"

#include "catch.hpp"

#include <QObject>
#include "ctkVTKObject.h"
#include "vtkIGTLIODevice.h"

namespace cxtest
{

class CXTEST_ORG_CUSTUSX_CORE_OPENIGTLINK3_EXPORT Receiver : public QObject
{
	Q_OBJECT
	QVTK_OBJECT

public:
	Receiver();
	virtual ~Receiver();

	void listen(vtkIGTLIODevicePointer device);

	int mEventsReceived;

signals:
	void done();

public slots:
	void onDeviceModified(vtkObject *, void *, unsigned long, void *);

};

}//namespace cxtest

#endif //CXTEST_RECEIVER_H
