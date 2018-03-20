/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTSENDER_H_
#define CXTESTSENDER_H_

#include "cxGrabberExport.h"

#include "cxSender.h"
#include <boost/shared_ptr.hpp>

namespace cxtest {

/*
 * TestSender
 *
 * Simulates sending images.
 * Can be used with a imagestreamer.
 *
 * \date May 13, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGrabber_EXPORT TestSender : public cx::Sender
{
	Q_OBJECT
public:
	TestSender() {}
	virtual ~TestSender() {}

	virtual bool isReady() const;

	virtual void send(cx::PackagePtr package);

	cx::PackagePtr getSentPackage();

signals:
	void newPackage();

private:
	cx::PackagePtr mPackage;

};
typedef boost::shared_ptr<TestSender> TestSenderPtr;

} /* namespace cxtest */
#endif /* CXTESTSENDER_H_ */
