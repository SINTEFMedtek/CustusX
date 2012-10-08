/*
 * cxImageSenderGE.h
 *
 *  Created on: Sep 19, 2012
 *      Author: olevs
 */

#ifndef CXIMAGESENDERGE_H_
#define CXIMAGESENDERGE_H_

#include <QObject> //needed for the mocer when OpenCv is not used...

#ifdef CX_USE_ISB_GE
#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QDateTime>
#include <QSize>
class QTimer;
#include "igtlImageMessage.h"
#include <QStringList>
#include "cxImageSenderFactory.h"
#include "../grabberCommon/cxIGTLinkImageMessage.h"

namespace cx
{
/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * This version uses the NTNU ISB data streamer module (provided by Gabriel Kiss)
 * to grab images from the E9 EG scanner (and similar)
 */
class ImageSenderGE: public ImageSender
{
public:
	Q_OBJECT
	ImageSenderGE(QObject* parent = NULL);
	virtual ~ImageSenderGE() {}

	virtual void initialize(StringMap arguments);
	virtual void startStreaming(QTcpSocket* socket);
	virtual void stopStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

protected:
private:
	QTcpSocket* mSocket;
	QTimer* mSendTimer;
	QTimer* mGrabTimer;
	StringMap mArguments;

	void initialize_local();
	void deinitialize_local();

private slots:
	void grab();
	void send();
};

}

#endif // CX_USE_ISB_GE
#endif // CXIMAGESENDERGE_H_
