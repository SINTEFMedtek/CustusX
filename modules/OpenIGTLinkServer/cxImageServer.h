/*
 * cxImageServer.h
 *
 *  \date Oct 30, 2010
 *      \author christiana
 */

#ifndef CXIMAGESERVER_H_
#define CXIMAGESERVER_H_

#include <QTcpServer>
#include <QTimer>
#include <QPointer>
#include "cxImageSender.h"

namespace cx
{

class ImageServer: public QTcpServer
{
Q_OBJECT

public:
	ImageServer(QObject* parent = NULL);
	virtual ~ImageServer();
	void startListen(int port);
	void printHelpText();
	void initialize();
protected:
	void incomingConnection(int socketDescriptor);
private slots:
	void socketDisconnectedSlot();
private:
	ImageSenderPtr mImageSender;
	QPointer<QTcpSocket> mSocket;
};

} // namespace cx

#endif /* CXIMAGESERVER_H_ */
