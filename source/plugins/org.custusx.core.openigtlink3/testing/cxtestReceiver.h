#ifndef CXTEST_RECEIVER_H
#define CXTEST_RECEIVER_H

#include "cxtest_org_custusx_core_openigtlink3_export.h"

#include "catch.hpp"

#include <QObject>
#include "ctkVTKObject.h"
#include "vtkIGTLIODevice.h"
#include "vtkIGTLIOLogic.h"

#include "cxImage.h"
#include "cxTransform3D.h"

namespace cxtest
{

class CXTEST_ORG_CUSTUSX_CORE_OPENIGTLINK3_EXPORT Receiver : public QObject
{
	Q_OBJECT
	QVTK_OBJECT

public:
	Receiver(vtkIGTLIOLogicPointer logic);
	virtual ~Receiver();

	void listen(vtkIGTLIODevicePointer device);

	int mEventsReceived;
	bool image_received;
	bool transform_received;

public slots:
	void onDeviceModified(vtkObject *caller, void *device, unsigned long event, void *);

private slots:
	void checkImage(cx::ImagePtr image);
	void checkTransform(QString devicename, cx::Transform3D transform, double timestamp);


};

}//namespace cxtest

#endif //CXTEST_RECEIVER_H
