#include "cxTestSender.h"

#include "sscTypeConversions.h"

namespace cxtest
{

static int i = 0;

bool TestSender::isReady() const
{
	return true;
}

void TestSender::send(cx::PackagePtr package)
{
	std::cout << string_cast(i) << std::endl;
	i++;
	mPackage = package;
	emit newPackage();
}

cx::PackagePtr TestSender::getPackage()
{
	return mPackage;
}

} /* namespace cxtest */
