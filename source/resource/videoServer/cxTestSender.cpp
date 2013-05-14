#include "cxTestSender.h"

namespace cx {

bool TestSender::isReady() const
{
	return true;
}

void TestSender::send(PackagePtr package)
{
	mPackage = package;
}

PackagePtr TestSender::getPackage()
{
	return mPackage;
}

} /* namespace cx */
