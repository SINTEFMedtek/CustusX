#ifndef CXTESTSENDER_H_
#define CXTESTSENDER_H_

#include "cxGrabberSender.h"
#include <boost/shared_ptr.hpp>

namespace cx {

/*
 * TestSender.h
 *
 * \date May 13, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class TestSender : public Sender
{
	Q_OBJECT
public:
	TestSender() {}
	virtual ~TestSender() {}

	virtual bool isReady() const;

	virtual void send(PackagePtr package);

	PackagePtr getPackage();

private:
	PackagePtr mPackage;
};
typedef boost::shared_ptr<TestSender> TestSenderPtr;
} /* namespace cx */
#endif /* CXTESTSENDER_H_ */
