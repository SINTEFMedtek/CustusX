#ifndef CXGRABBERSENDERDIRECTLINK_H_
#define CXGRABBERSENDERDIRECTLINK_H_

#include "cxGrabberSender.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include "sscImage.h"
#include "sscTool.h"
//#include <qtcpsocket.h>
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"

namespace cx
{

class GrabberSenderDirectLink : public SenderImpl
{
	Q_OBJECT

public:
	GrabberSenderDirectLink() {}
	virtual ~GrabberSenderDirectLink() {}

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
typedef boost::shared_ptr<GrabberSenderDirectLink> GrabberSenderDirectLinkPtr;

}//namespace cx
#endif /* CXGRABBERSENDERDIRECTLINK_H_ */
