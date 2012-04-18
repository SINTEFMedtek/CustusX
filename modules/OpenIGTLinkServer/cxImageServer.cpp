/*
 * cxImageServer.cpp
 *
 *  \date Oct 30, 2010
 *      \author christiana
 */
#include "cxImageServer.h"
#include "cxImageSession.h"
#include <iostream>
#include <QCoreApplication>

namespace cx
{

ImageServer::ImageServer(QObject* parent) :
	QTcpServer(parent)
{
	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(tick())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	mTimer->start(500);

	QuitCheck* quitChecker = new QuitCheck();
	quitChecker->start();
}

void ImageServer::initialize()
{
	StringMap args = cx::extractCommandlineOptions(QCoreApplication::arguments());

	QString type = ImageSenderFactory().getDefaultSenderType();
	if (args.count("type"))
		type = args["type"];

	mImageSender = ImageSenderFactory().getImageSender(type);

	if (mImageSender)
	{
		std::cout << "Success: Created sender of type: " << type.toStdString() << std::endl;
	}
	else
	{
		std::cout << "Error: Failed to create sender based on type: " << type.toStdString() << std::endl;
	}

	mImageSender->initialize(args);
}

void ImageServer::tick()
{
////	return;
////	char s;
////	std::cin.read(&s, 1);
//	// if any input detected: quit application
//	std::string val;
//	std::cout << "dddd" << std::endl;
//	std::cin >> val;
////	if (s=='q')
//	if (!val.empty())
//	{
//		std::cout << "Close server..." << std::endl;
//		this->socketDisconnectedSlot();
//		qApp->quit();
//	}
}

void ImageServer::startListen(int port)
{
	bool started = this->listen(QHostAddress::Any, port);

	if (started)
	{
		if (this->serverAddress().isNull())
			std::cout << QString("Server is listening to port %2").arg(this->serverPort()).toStdString() << std::endl;
		else
			std::cout << QString("Server %1 is listening to port %2").arg(this->serverAddress().toString()).arg(
				this->serverPort()).toStdString() << std::endl;
	}
	else
		std::cout << "Server failed to start. Error: " << this->errorString().toStdString() << std::endl;
}

ImageServer::~ImageServer()
{
}

void ImageServer::incomingConnection(int socketDescriptor)
{
	std::cout << "Server: Incoming connection..." << std::endl;

	if (mSocket != 0)
	{
		std::cout << "Server error: Can only handle a single connection." << std::endl;
		return;
	}

	mSocket = new QTcpSocket();
	connect(mSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnectedSlot()));
	mSocket->setSocketDescriptor(socketDescriptor);
	QString clientName = mSocket->localAddress().toString();
	std::cout << "Connected to " << clientName.toStdString() << ". Session started." << std::endl;

	mImageSender->startStreaming(mSocket);
}

void ImageServer::socketDisconnectedSlot()
{
	if (mImageSender)
		mImageSender->stopStreaming();

	if (mSocket)
	{
		QString clientName = mSocket->localAddress().toString();
		std::cout << "Disconnected from " << clientName.toStdString() << ". Session ended." << std::endl;
		mSocket->deleteLater();
	}
}

void ImageServer::printHelpText()
{
	StringMap args = cx::extractCommandlineOptions(QCoreApplication::arguments());
	cx::ImageSenderFactory factory;

	std::cout << "Usage: " << qApp->applicationName().toStdString() << " (--arg <argval>)*" << std::endl;
	std::cout << "    --port   : Tcp/IP port # (default=18333)" << std::endl;
	std::cout << "    --type   : Grabber type  (default=" << factory.getDefaultSenderType().toStdString() << ")"
		<< std::endl;
	std::cout << std::endl;
	std::cout << "    Select one of the types below:" << std::endl;

	QStringList types = factory.getSenderTypes();
	for (int i = 0; i < types.size(); ++i)
	{
		QStringList args = factory.getArgumentDescription(types[i]);
		std::cout << std::endl;
		std::cout << "      type = " << types[i].toStdString() << std::endl;
		for (int j = 0; j < args.size(); ++j)
			std::cout << "        " << args[j].toStdString() << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Press any key + Enter to close the server."<< std::endl;
	std::cout << std::endl;
}
}
