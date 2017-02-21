/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CX_NETWORKHANDLER_H_
#define CX_NETWORKHANDLER_H_

#include "org_custusx_core_openigtlink3_Export.h"
#include "igtlioLogic.h"
#include "igtlioSession.h"

#include "cxTransform3D.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxProbeDefinition.h"

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

signals:
	void connected();
	void disconnected();

	void transform(QString devicename, Transform3D transform, double timestamp);
	void image(ImagePtr image);
	void commandRespons(QString devicename, QString xml);
	//void mesh(MeshPtr image);
	void probedefinition(QString devicename, ProbeDefinitionPtr definition);
	//void calibration(QString devicename, Transform3D calibration);

private slots:
	void onConnectionEvent(vtkObject* caller, void* connector, unsigned long event, void*);
	void onDeviceAddedOrRemoved(vtkObject* caller, void* connector, unsigned long event, void*callData);
	void onDeviceModified(vtkObject * caller_device, void * unknown, unsigned long event, void *);
	void periodicProcess();

private:
	void connectToConnectionEvents();
	void connectToDeviceEvents();

	igtlio::LogicPointer mLogic;
	igtlio::SessionPointer mSession;
	QTimer *mTimer;
	void hackEmitProbeDefintionForPlusTestSetup(QString deviceName);
};

} // namespace cx

#endif /* CX_NETWORKHANDLER_H_ */
