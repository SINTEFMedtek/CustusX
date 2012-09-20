/*
 * cxImageSenderGE.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: olevs
 */

#include "cxImageSenderGE.h"

#ifdef CX_USE_ISB_GE

#include <QCoreApplication>
#include <QTimer>
#include <QTime>
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
#include "DataStreamApp.h"

namespace cx
{

QString ImageSenderGE::getType()
{
	return "ISB_GE";
}

QStringList ImageSenderGE::getArgumentDescription()
{
	QStringList retval;
//	retval << "--videoport:  video id,     default=0";
//	retval << "--height:     image height, default=camera";
//	retval << "--width:      image width,  default=camera";
//	retval << "--properties: dump image properties";
	return retval;
}

ImageSenderGE::ImageSenderGE(QObject* parent) :
	ImageSender(parent),
	mSocket(NULL),
	mSendTimer(0),
	mGrabTimer(0)
{
	data_streaming::DataStreamApp test;

	mGrabTimer = new QTimer(this);
	connect(mGrabTimer, SIGNAL(timeout()), this, SLOT(grab())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	mSendTimer = new QTimer(this);
	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(send())); // this signal will be executed in the thread of THIS, i.e. the main thread.
}

void ImageSenderGE::initialize(StringMap arguments)
{
	mArguments = arguments;

	// Run an init/deinit to check that we have contact right away.
	// Do NOT keep the connection open: This is because we have no good way to
	// release resources if the server is a local app and is killed by CustusX.
	// This way, we can disconnect (thus releasing resources), and then safely
	// remove the usb cable without having dangling resources in openCV. (problem at least on Linux)
//	this->initialize_local();
//	this->deinitialize_local();
}

void ImageSenderGE::deinitialize_local()
{

}

void ImageSenderGE::initialize_local()
{

}

void ImageSenderGE::startStreaming(QTcpSocket* socket)
{
	this->initialize_local();

	if (!mGrabTimer || !mSendTimer)
	{
		std::cout << "ImageSenderGE: Failed to start streaming: Not initialized." << std::endl;
		return;
	}

	mSocket = socket;
	mGrabTimer->start(0);
	mSendTimer->start(40);
}

void ImageSenderGE::stopStreaming()
{
	if (!mGrabTimer || !mSendTimer)
		return;
	mGrabTimer->stop();
	mSendTimer->stop();
	mSocket = NULL;

	this->deinitialize_local();
}

void ImageSenderGE::grab()
{
}

void ImageSenderGE::send()
{
}
}

#endif //CX_USE_ISB_GE
