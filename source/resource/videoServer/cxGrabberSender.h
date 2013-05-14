#ifndef CXGRABBERSENDER_H_
#define CXGRABBERSENDER_H_

#include <QObject>
#include <boost/shared_ptr.hpp>
#include <qtcpsocket.h>
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "sscImage.h"
#include "sscTool.h"

namespace cx
{

struct Package
{
	IGTLinkImageMessage::Pointer mIgtLinkImageMessage;
	IGTLinkUSStatusMessage::Pointer mIgtLinkUSStatusMessage;
	ssc::ImagePtr mImage;
	ssc::ProbeDataPtr mProbe;

};

typedef boost::shared_ptr<Package> PackagePtr;

/**Interface for sending grabbed data from the ImageSender classes further to either
 * a TCP/IP socket or directly via a Qt signal.
 * \author Christian Askeland, SINTEF
 * \date Oct 11, 2012
 */
class Sender : public QObject
{
	Q_OBJECT
public:
	Sender() {}
	virtual ~Sender() {}

	/**Return if sender is ready to send another message. If !isReady(),
	 * calls to send() will fail.
	 */
	virtual bool isReady() const = 0;

	virtual void send(PackagePtr package) = 0;

};
typedef boost::shared_ptr<Sender> SenderPtr;

class SenderImpl : public Sender
{
public:
	SenderImpl() {}
	virtual ~SenderImpl() {}

	/**Return if sender is ready to send another message. If !isReady(),
	 * calls to send() will fail.
	 */
	virtual bool isReady() const = 0;

	virtual void send(PackagePtr package);

protected:
	/**Send an image message, NOT packed.
	 */
	virtual void send(IGTLinkImageMessage::Pointer msg) = 0;
	/**Send an US status message, NOT packed
	 */
	virtual void send(IGTLinkUSStatusMessage::Pointer msg) = 0;
	/**Send an image message, NOT packed.
	 */
	virtual void send(ssc::ImagePtr msg) = 0;
	/**Send an US status message, NOT packed
	 */
	virtual void send(ssc::ProbeDataPtr msg) = 0;
};

class GrabberSenderQTcpSocket : public SenderImpl
{
public:
	explicit GrabberSenderQTcpSocket(QTcpSocket* socket);
	virtual ~GrabberSenderQTcpSocket() {}

	bool isReady() const;

protected:
	virtual void send(IGTLinkImageMessage::Pointer msg);
	virtual void send(IGTLinkUSStatusMessage::Pointer msg);
	virtual void send(ssc::ImagePtr msg);
	virtual void send(ssc::ProbeDataPtr msg);

private:
	QTcpSocket* mSocket;
	int mMaxBufferSize;
};

} /* namespace cx */
#endif /* CXGRABBERSENDER_H_ */
