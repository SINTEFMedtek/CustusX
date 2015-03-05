#include "cxOpenIGTLinkSessionManager.h"


#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlMessageHandlerMacro.h"
#include "igtlSessionManager.h"
#include "igtlTransformMessage.h"

#include "cxLogger.h"

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

//-----------------------------------------------------------------------------

static MyData MYDATA;

OpenIGTLinkSessionManager::OpenIGTLinkSessionManager(QObject *parent) :
    QObject(parent), mState(Idle)
{
    const char* hostname = "10.218.140.127";
    int port = 18944;

    mSessionManager = igtl::SessionManager::New();
    mSessionManager->SetMode(igtl::SessionManager::MODE_CLIENT);
    mSessionManager->SetHostname(hostname);
    mSessionManager->SetPort(port);
    this->addMessageHandlers();
}

void OpenIGTLinkSessionManager::connectAndListen()
{
    if (mSessionManager->Connect())
    {
        mState = Connected;
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "Connected to server: " << mSessionManager->GetHostname();
        this->listen();
        mSessionManager->Disconnect();
    }
}

void OpenIGTLinkSessionManager::addMessageHandlers()
{
    static TransformHandler::Pointer tmh = TransformHandler::New();
    tmh->SetData(&MYDATA);
    this->addMessageHandler(tmh);
}

void OpenIGTLinkSessionManager::addMessageHandler(igtl::MessageHandler *handler)
{
    mSessionManager->AddMessageHandler(handler);
}

void OpenIGTLinkSessionManager::listen()
{
    mState = Listening;

    while(mState == Listening)
    {
        int r = mSessionManager->ProcessMessage();
        if (r == 0) // Disconnected
        {
            mState = Idle;
        }
        else if(r == -1) //Error
        {
            CX_LOG_CHANNEL_ERROR("janne beate ") << "The session manager could not process the opentigtlink message.";
        }
        else if(r == 1) //OK
        {
            CX_LOG_CHANNEL_DEBUG("janne beate ") << "MyData: devicename " << MYDATA.devicename << " messagetype " << MYDATA.messagetype;
            //TODO emit the transform
        }
    }
}
} //namespace cx
