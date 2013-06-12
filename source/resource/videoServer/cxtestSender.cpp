#include "cxtestSender.h"

#include "sscTypeConversions.h"

namespace cxtest
{

bool TestSender::isReady() const
{
	return true;
}

void TestSender::send(cx::PackagePtr package)
{
	mPackage = package;
	emit newPackage();
}

cx::PackagePtr TestSender::getSentPackage()
{
	return mPackage;
}

} /* namespace cxtest */
