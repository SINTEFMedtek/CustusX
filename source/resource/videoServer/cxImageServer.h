#ifndef CXIMAGESERVER_H_
#define CXIMAGESERVER_H_

#include <QTcpServer>
#include <QTimer>
#include <QPointer>
#include "cxImageStreamer.h"

namespace cx
{

/**
 * \brief ImageServer
 *
 * \date Oct 30, 2010
 * \author Christian Askeland
 */

class ImageServer: public QTcpServer
{
	Q_OBJECT

public:
	ImageServer(QObject* parent = NULL);
	virtual ~ImageServer();
	bool startListen(int port);
	static void printHelpText();
	static QString getArgumentHelpText(QString applicationName);
	bool initialize();
protected:
	void incomingConnection(int socketDescriptor);
private slots:
	void socketDisconnectedSlot();
private:
	StreamerPtr mImageSender;
	QPointer<QTcpSocket> mSocket;
};

} // namespace cx

#endif /* CXIMAGESERVER_H_ */
