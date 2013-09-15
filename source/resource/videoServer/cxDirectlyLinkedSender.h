#ifndef CXDirectlyLinkedSender_H_
#define CXDirectlyLinkedSender_H_

#include "cxSender.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include "sscImage.h"
#include "sscTool.h"
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"

namespace cx
{

class DirectlyLinkedSender : public SenderImpl
{
	Q_OBJECT

public:
	DirectlyLinkedSender() {}
	virtual ~DirectlyLinkedSender() {}

	bool isReady() const;
	virtual void send(IGTLinkImageMessage::Pointer msg);
	virtual void send(IGTLinkUSStatusMessage::Pointer msg);
	virtual void send(ImagePtr msg);
	virtual void send(ProbeDataPtr msg);

	ImagePtr popImage();
	ProbeDataPtr popUSStatus();

signals:
	void newImage();
	void newUSStatus();

private:
	ImagePtr mImage;
	ProbeDataPtr mUSStatus;
	IGTLinkUSStatusMessage::Pointer mUnsentUSStatusMessage; ///< received message, will be added to queue when next image arrives

};
typedef boost::shared_ptr<DirectlyLinkedSender> DirectlyLinkedSenderPtr;

}//namespace cx
#endif /* CXDirectlyLinkedSender_H_ */
