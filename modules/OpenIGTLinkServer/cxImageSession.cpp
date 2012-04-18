/*
 * cxImageServer.cpp
 *
 *  \date Oct 30, 2010
 *      \author christiana
 */
#include "cxImageSession.h"

#include <QTimer>
#include <QHostAddress>
#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageImport.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkMetaImageWriter.h"
#include <QCoreApplication>

typedef vtkSmartPointer<vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<vtkLookupTable> vtkLookupTablePtr;

#include "cxImageSenderFile.h"
#include "cxImageSenderOpenCV.h"

namespace cx
{

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------


//ImageSession::ImageSession(int socketDescriptor, ImageSenderPtr imageSender, QObject* parent) :
//	QThread(parent), mSocketDescriptor(socketDescriptor), mImageSender(imageSender)
//{
////	mArguments = cx::extractCommandlineOptions(QCoreApplication::arguments());
//}
//
//ImageSession::~ImageSession()
//{
//}
//
//void ImageSession::run()
//{
//	mSocket = new QTcpSocket();
//	connect(mSocket, SIGNAL(disconnected()), this, SLOT(quit()), Qt::DirectConnection); // quit thread when disconnected
//	mSocket->setSocketDescriptor(mSocketDescriptor);
//	QString clientName = mSocket->localAddress().toString();
//	std::cout << "Connected to " << clientName.toStdString() << ". Session started." << std::endl;
//
////	std::cout << mArguments["type"].toStdString().c_str() << std::endl;
//
////	ImageSenderPtr sender = ImageSenderFactory().getImageSender(mArguments["type"]);
////	sender->initialize(mArguments);
//	mImageSender->startStreaming(mSocket);
//
////	QObject* sender = ImageSenderFactory().createSender(mArguments["type"], mSocket, mArguments);
//
//	if (mImageSender)
//	{
//		this->exec();
//		mImageSender->stopStreaming();
//	}
//	else
//	{
//		std::cout << "Failed to create sender based on arg " << mArguments["type"].toStdString() << std::endl;
//	}
//
//	std::cout << "Disconnected from " << clientName.toStdString() << ". Session ended." << std::endl;
////	delete sender;
//	delete mSocket;
//}

} // cx
