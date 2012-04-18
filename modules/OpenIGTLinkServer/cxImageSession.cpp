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

/** streambuf subclass: used to override either cout or cerr and
 * reroute text to both messagemanager and original stream.
 *
 */
class MyStreamBuf: public std::basic_streambuf<char, std::char_traits<char> >
{
public:
  MyStreamBuf() :
    mEnabledRedirect(true), mOrig(NULL)
  {
  }
  void setOriginal(std::streambuf* orig)
  {
    mOrig = orig;
  }
  virtual int_type overflow(int_type meta = traits_type::eof())
  {
    char single = traits_type::to_char_type(meta);
//    if (mOrig) // send to original stream as well
//      mOrig->sputc(single);

    if (mEnabledRedirect)
    {
    	std::cout << "got " << single << std::endl;
//      if (single == '\n')
//      {
//        QMutexLocker sentry(&mMutex);
//        QString buffer = qstring_cast(mBuffer);
//        mBuffer.clear();
//        sentry.unlock();
//
//        messageManager()->sendMessage(buffer, mMessageLevel, 0);
//      }
//      else
//      {
//        QMutexLocker sentry(&mMutex);
//        mBuffer += single;
//      }
    }
    return traits_type::not_eof(meta);
  }
  void setEnableRedirect(bool on)
  {
    mEnabledRedirect = on;
  }

  //    virtual int_type overflow(int_type meta=traits_type::eof());
private:
  bool mEnabledRedirect;
  QString mBuffer;
  std::streambuf* mOrig;
//  QMutex mMutex;
};

class SingleStreamerImpl
{
private:
  boost::shared_ptr<class MyStreamBuf> StreamBuf;
  std::streambuf *OrigBuf;
  std::istream& mStream;
public:
  SingleStreamerImpl(std::istream& str) :
    mStream(str)
  {
	  std::cout << "SingleStreamerImpl starting ...." << std::endl;
    StreamBuf.reset(new MyStreamBuf());
    OrigBuf = mStream.rdbuf(StreamBuf.get());
    StreamBuf->setOriginal(OrigBuf);
  }
  ~SingleStreamerImpl()
  {
    mStream.rdbuf(OrigBuf);
  }
  void setEnableRedirect(bool on)
  {
    StreamBuf->setEnableRedirect(on);
  }

};

QuitCheck::QuitCheck(QObject* parent) : QThread(parent)
{
	  new SingleStreamerImpl(std::cin);
}

void QuitCheck::run()
{
	std::cout << "tick" << std::endl;

	while(true)
	{
		std::string val;
		std::cin >> val;
		if (!val.empty())
		{
			std::cout << "Close server..." << std::endl;
			qApp->quit();
			return;
		}
	}
	//	return;
	//	char s;
	//	std::cin.read(&s, 1);
		// if any input detected: quit application
//		std::string val;
////		std::cout << "dddd" << std::endl;
//		std::cin >> val;
//	//	if (s=='q')
//		if (!val.empty())
//		{
//			std::cout << "Close server..." << std::endl;
//			this->socketDisconnectedSlot();
//			qApp->quit();
//		}
}

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
