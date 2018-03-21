/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMAGESERVER_H_
#define CXIMAGESERVER_H_

#include "cxGrabberExport.h"

#include <QTcpServer>
#include <QTimer>
#include <QPointer>
#include "boost/shared_ptr.hpp"

namespace cx
{
typedef boost::shared_ptr<class Streamer> StreamerPtr;

/**
 * \brief ImageServer
 *
 * \ingroup cx_resource_videoserver
 * \date Oct 30, 2010
 * \author Christian Askeland
 */

class cxGrabber_EXPORT ImageServer: public QTcpServer
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
	void incomingConnection(qintptr socketDescriptor);
private slots:
	void socketDisconnectedSlot();
private:
	StreamerPtr mImageSender;
	QPointer<QTcpSocket> mSocket;
};

} // namespace cx

#endif /* CXIMAGESERVER_H_ */
