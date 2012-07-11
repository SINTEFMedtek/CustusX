// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscRepImpl.h"
#include "sscTypeConversions.h"

namespace ssc
{

RepImpl::RepImpl(const QString& uid, const QString& name) :
	mName(name), mUid(uid)
{
}

RepImpl::~RepImpl()
{
}

void RepImpl::setName(QString name)
{
	mName = name;
}

QString RepImpl::getName() const
{
	return mName;
}

QString RepImpl::getUid() const
{
	return mUid;
}

bool RepImpl::isConnectedToView(View *theView) const
{
	return mViews.count(theView);
}

void RepImpl::connectToView(View *theView)
{
	mViews.insert(theView);
	this->addRepActorsToViewRenderer(theView);
}

void RepImpl::disconnectFromView(View *theView)
{
	mViews.erase(theView);
	this->removeRepActorsFromViewRenderer(theView);
}

void RepImpl::printSelf(std::ostream & os, Indent indent)
{
	os << indent << "mUid: " << mUid << std::endl;
	os << indent << "mName: " << mName << std::endl;
	os << indent << "Type: " << getType() << std::endl;
}


} // namespace ssc
