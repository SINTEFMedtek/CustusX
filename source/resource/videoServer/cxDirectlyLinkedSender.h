/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDirectlyLinkedSender_H_
#define CXDirectlyLinkedSender_H_

#include "cxGrabberExport.h"

#include "cxSenderImpl.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include "cxImage.h"
#include "cxTool.h"
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"

namespace cx
{

/**
 * \ingroup cx_resource_videoserver
 *
 */
class cxGrabber_EXPORT DirectlyLinkedSender : public SenderImpl
{
	Q_OBJECT

public:
	DirectlyLinkedSender() {}
	virtual ~DirectlyLinkedSender() {}

	bool isReady() const;
	virtual void send(ImagePtr msg);
	virtual void send(ProbeDefinitionPtr msg);

	ImagePtr popImage();
	ProbeDefinitionPtr popUSStatus();

signals:
	void newImage();
	void newUSStatus();

private:
	ImagePtr mImage;
	ProbeDefinitionPtr mUSStatus;

};
typedef boost::shared_ptr<DirectlyLinkedSender> DirectlyLinkedSenderPtr;

}//namespace cx
#endif /* CXDirectlyLinkedSender_H_ */
