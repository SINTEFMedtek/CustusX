/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CX_NETWORKHANDLER_H_
#define CX_NETWORKHANDLER_H_

#include "org_custusx_core_openigtlink3_Export.h"
#include "igtlioLogic.h"
#include "igtlioSession.h"

#include "cxTransform3D.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxProbeDefinitionFromStringMessages.h"

#include "ctkVTKObject.h"

namespace cx
{

typedef boost::shared_ptr<class NetworkHandler> NetworkHandlerPtr;

class org_custusx_core_openigtlink3_EXPORT NetworkHandler : public QObject
{
	Q_OBJECT
	QVTK_OBJECT

public:
	NetworkHandler(igtlio::LogicPointer logic);
	~NetworkHandler();

	igtlio::SessionPointer requestConnectToServer(std::string serverHost, int serverPort=-1, igtlio::SYNCHRONIZATION_TYPE sync=igtlio::BLOCKING, double timeout_s=5);
	void disconnectFromServer();

signals:
	void connected();
	void disconnected();

	void transform(QString devicename, Transform3D transform, double timestamp);
	void image(ImagePtr image);
	void commandRespons(QString devicename, QString xml);
	void string_message(QString message);
	//void mesh(MeshPtr image);
	void probedefinition(QString devicename, ProbeDefinitionPtr definition);
	//void calibration(QString devicename, Transform3D calibration);

private slots:
	void onConnectionEvent(vtkObject* caller, void* connector, unsigned long event, void*);
	void onDeviceAddedOrRemoved(vtkObject* caller, void* connector, unsigned long event, void*callData);
	void onDeviceReceived(vtkObject * caller_device, void * unknown, unsigned long event, void *);
	void periodicProcess();

private:
	void connectToConnectionEvents();
	void connectToDeviceEvents();

	igtlio::LogicPointer mLogic;
	igtlio::SessionPointer mSession;
	QTimer *mTimer;
	ProbeDefinitionFromStringMessagesPtr mProbeDefinitionFromStringMessages;
};

} // namespace cx

#endif /* CX_NETWORKHANDLER_H_ */
