/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSender_H_
#define CXSender_H_

#include "cxGrabberExport.h"

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

struct Package
{
	ImagePtr mImage;
	ProbeDefinitionPtr mProbe;
};

typedef boost::shared_ptr<Package> PackagePtr;

/** Interface for sending grabbed data from the ImageSender classes further to either
 *  a TCP/IP socket or directly via a Qt signal.
 *
 * \ingroup cx_resource_videoserver
 * \author Christian Askeland, SINTEF
 * \date Oct 11, 2012
 */
class cxGrabber_EXPORT Sender : public QObject
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

/**
* @}
*/

} /* namespace cx */
#endif /* CXSender_H_ */
