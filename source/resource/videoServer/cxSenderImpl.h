/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSenderImpl_H_
#define CXSenderImpl_H_

#include "cxGrabberExport.h"

#include "cxSender.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include <qtcpsocket.h>
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxImage.h"
#include "cxTool.h"

namespace cx
{

/**
* \file
* \addtogroup cx_resource_videoserver
* @{
*/

class cxGrabber_EXPORT SenderImpl : public Sender
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
	/** Send an image message
	 */
	virtual void send(ImagePtr msg) = 0;
	/** Send an US status message
	 */
	virtual void send(ProbeDefinitionPtr msg) = 0;
};

/**
* @}
*/

} /* namespace cx */
#endif /* CXSenderImpl_H_ */
