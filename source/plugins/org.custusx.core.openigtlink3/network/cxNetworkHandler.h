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
#include "qfuturewatcher.h"
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

struct ThreadResult
{
	QString deviceName;
	ImagePtr image;
	ProbeDefinitionPtr probeDefinition;
	bool probeDefinitionHaveChanged;
	vtkImageDataPtr USMask;
	ProbeDefinitionFromStringMessagesPtr probeDefinitionFromStringMessages;
	bool zeroesInImage;
	int skippedImages;
	int sentNumProbeDefinitions = 0;
	bool shouldEmitProbeDefinition()
	{
		bool retval = probeDefinitionHaveChanged;
		if(sentNumProbeDefinitions < 2)
		{
			sentNumProbeDefinitions++;
			retval = true;
		}
		return retval;
	}
};

class org_custusx_core_openigtlink3_EXPORT NetworkHandler : public QObject
{
	Q_OBJECT
	QVTK_OBJECT

public:
	NetworkHandler(igtlioLogicPointer logic);
	~NetworkHandler();

	igtlioSessionPointer requestConnectToServer(std::string serverHost, int serverPort=-1, IGTLIO_SYNCHRONIZATION_TYPE sync=IGTLIO_BLOCKING, double timeout_s=5);
	void disconnectFromServer();

	void clearTimestampSynchronization();

signals:
	void connected();
	void disconnected();

	void transform(QString devicename, Transform3D transform, double timestamp);
	void image(ImagePtr image);
	void commandRespons(QString devicename, QString xml);
	void string_message(QString message);
	void probedefinition(QString devicename, ProbeDefinitionPtr definition);

public slots:
	void resendProbedefinition();

protected slots:
	void onConnectionEvent(vtkObject* caller, void* connector, unsigned long event, void*);
	void onDeviceAddedOrRemoved(vtkObject* caller, void* connector, unsigned long event, void*callData);
	void onDeviceReceived(vtkObject * caller_device, void * unknown, unsigned long event, void *);
	void periodicProcess();
	void futureImageFinished();

protected:
	void connectToConnectionEvents();
	void connectToDeviceEvents();
	ThreadResult processImage(ThreadResult result);
	bool convertZeroesInsideSectorToOnes(ThreadResult &result, int threshold = 0, int newValue = 1);
	bool createMask(ThreadResult &result);
	double synchronizedTimestamp(double receivedTimestampSec);///Synchronize with system clock: Calculate a fixed offset, and apply this to all timestamps
	bool verifyTimestamp(double &timestampMS);
	void saveThreadResult(ThreadResult &threadResult);
	ThreadResult createThreadResultObject(QString deviceName, ImagePtr image = ImagePtr());

	igtlioLogicPointer mLogic;
	igtlioSessionPointer mSession;//, mSession2;
	QTimer *mTimer;

	bool mGotTimeOffset;
	double mTimestampOffsetMS;

	std::list<QFutureWatcher<ThreadResult>*> mSaveImageThreads;
	std::map<QString, ThreadResult> mDevices;
};

} // namespace cx

#endif /* CX_NETWORKHANDLER_H_ */
