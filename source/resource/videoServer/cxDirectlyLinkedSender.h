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
	virtual void send(ssc::ImagePtr msg);
	virtual void send(ssc::ProbeDataPtr msg);

	ssc::ImagePtr popImage();
	ssc::ProbeDataPtr popUSStatus();

signals:
	void newImage();
	void newUSStatus();

private:
	ssc::ImagePtr mImage;
	ssc::ProbeDataPtr mUSStatus;
	IGTLinkUSStatusMessage::Pointer mUnsentUSStatusMessage; ///< received message, will be added to queue when next image arrives

};
typedef boost::shared_ptr<DirectlyLinkedSender> DirectlyLinkedSenderPtr;

}//namespace cx
#endif /* CXDirectlyLinkedSender_H_ */
