#include "cxOpenIGTLinkSessionManager.h"

#include <QCoreApplication>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlMessageHandlerMacro.h"
#include "igtlSessionManager.h"
#include "igtlTransformMessage.h"
#include "igtlStatusMessage.h"
#include "igtlImageMessage.h"

#include "cxLogger.h"
#include "cxUtilHelpers.h"

namespace cx {

//------------------------------------------------------------
// Define a structure type to share data between message
// handler classes and main() function.
// It can be any types e.g. C++ class, array, etc.
// In this example, the shared structure is only used for
// passing the message type and the device name from the
// handler to main() function.

typedef struct {
  std::string messagetype;
  std::string devicename;
} MyData;


//------------------------------------------------------------
// Define message handler classes for TransformMessage,
// PositionMessage and ImageMessage.
// igtlMessageHandlerClassMacro() defines a child class of
// igtl::MessageHandler to handle OpenIGTLink messages for
// the message type specified as the first argument. The
// second argument will be used for the name of this
// message handler class, while the third argument specifies
// a type of data that will be shared with the message functions
// of this handler class.

igtlMessageHandlerClassMacro(igtl::TransformMessage, TransformHandler, MyData); //TODO: put into Transform3D instead of MyData
igtlMessageHandlerClassMacro(igtl::StatusMessage, StatusHandler, MyData);
igtlMessageHandlerClassMacro(igtl::ImageMessage, ImageHandler, MyData);


//------------------------------------------------------------
// You need to describe how the received message is processed
// in Process() function of the message handler class.
// When Process() is called, pointers to the received message
// and the shared data are passed as the arguments.

// -- Transform message
int TransformHandler::Process(igtl::TransformMessage * transMsg, MyData* data)
{
  // Retrive the transform data
  igtl::Matrix4x4 matrix;
  transMsg->GetMatrix(matrix);
  igtl::PrintMatrix(matrix);

  data->messagetype = transMsg->GetDeviceType();
  data->devicename  = transMsg->GetDeviceName();

  return 1;
}

// -- Status message
int StatusHandler::Process(igtl::StatusMessage * statusMsg, MyData* data)
{
  data->messagetype = statusMsg->GetDeviceType();
  data->devicename  = statusMsg->GetDeviceName();
  std::cout << "STATUS code: " << statusMsg->GetCode() << std::endl;
  if(statusMsg->GetCode() != 1) //1 is OK
  {
      std::cout << "STATUS string: " << statusMsg->GetStatusString() << std::endl;
      std::cout << "STATUS sub code: " << statusMsg->GetSubCode() << std::endl;
      std::cout << "STATUS error name: " << statusMsg->GetErrorName() << std::endl;
  }

  return 1;
}

// -- Image message
int ImageHandler::Process(igtl::ImageMessage * imageMsg, MyData* data)
{
  data->messagetype = imageMsg->GetDeviceType();
  data->devicename  = imageMsg->GetDeviceName();
  std::cout << "IMAGE size: " << imageMsg->GetImageSize() << std::endl;

  return 1;
}

//-----------------------------------------------------------------------------

static MyData TRANSFORM_DATA;
static MyData STATUS_DATA;
static MyData IMAGE_DATA;

OpenIGTLinkSessionManager::OpenIGTLinkSessionManager(QObject *parent) :
    QObject(parent), mState(Idle)
{
    mSessionManager = igtl::SessionManager::New();
    mSessionManager->SetMode(igtl::SessionManager::MODE_CLIENT);
    this->addMessageHandlers();
}

void OpenIGTLinkSessionManager::requestConnect(QString ip, int port)
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "OpenIGTLinkSessionManager is trying to connect to " << ip << ":" << port;
    mSessionManager->SetHostname(ip.toStdString().c_str());
    mSessionManager->SetPort(port);
    if (mSessionManager->Connect())
    {
        this->internalConnected();
    }
    else
    {
        CX_LOG_ERROR() << "Could not connect to an OpenIGTLinkSever at " << ip << ":" << port;
    }
}

void OpenIGTLinkSessionManager::requestDisconnect()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "request diconnect";
    mSessionManager->Disconnect();
    this->internalDisconnected();
}

void OpenIGTLinkSessionManager::requestStartProcessingMessages()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "request start processing";
    this->internalStartedProcessingMessages();
    this->listen();
}

void OpenIGTLinkSessionManager::requestStopProcessingMessages()
{
    CX_LOG_CHANNEL_DEBUG("janne beate 2") << "request stop";
     this->internalStoppedProcessingMessages();
}

void OpenIGTLinkSessionManager::addMessageHandlers()
{
    static TransformHandler::Pointer tmh = TransformHandler::New();
    static StatusHandler::Pointer smh = StatusHandler::New();
    static ImageHandler::Pointer imh = ImageHandler::New();
    tmh->SetData(&TRANSFORM_DATA);
    smh->SetData(&STATUS_DATA);
    imh->SetData(&IMAGE_DATA);
    this->addMessageHandler(tmh);
    this->addMessageHandler(smh);
    this->addMessageHandler(imh);
}

void OpenIGTLinkSessionManager::addMessageHandler(igtl::MessageHandler *handler)
{
    mSessionManager->AddMessageHandler(handler);
}

void OpenIGTLinkSessionManager::listen()
{
    while(mState == Listening)
    {
        int r = mSessionManager->ProcessMessage();
        if (r == 0) // Disconnected
        {
            this->requestDisconnect();
        }
        else if(r == -1) //Error: only part of the header has arrived
        {
            CX_LOG_CHANNEL_ERROR("janne beate ") << "The session manager could not process the opentigtlink message.";
            this->requestStopProcessingMessages();
        }
        else if(r == 1) //OK
        {
            //CX_LOG_CHANNEL_DEBUG("janne beate ") << "MyData: devicename " << MYDATA.devicename << " messagetype " << MYDATA.messagetype;
            //TODO emit the transform
        }
        QCoreApplication::processEvents();
        sleep_ms(100);
    }
}
void OpenIGTLinkSessionManager::internalConnected()
{
    mState = Connected;
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Connected to server: " << mSessionManager->GetHostname();
    emit connected();
}

void OpenIGTLinkSessionManager::internalDisconnected()
{
    mState = Idle;
    emit disconnected();
}

void OpenIGTLinkSessionManager::internalStartedProcessingMessages()
{
    mState = Listening;
    emit startedProcessingMessages();
}

void OpenIGTLinkSessionManager::internalStoppedProcessingMessages()
{
    mState = Connected;
    emit stoppedProcessingMessages();
}

} //namespace cx
