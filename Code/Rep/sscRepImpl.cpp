#include "sscRepImpl.h"

namespace ssc
{

RepImpl::RepImpl(const std::string& uid, const std::string& name) :
	mName(name), mUid(uid)
{
}

RepImpl::~RepImpl()
{
}

void RepImpl::setName(std::string name)
{
	mName = name;
}

std::string RepImpl::getName() const
{
	return mName;
}

std::string RepImpl::getUid() const
{
	return mUid;
}

bool RepImpl::isConnectedToView(View * theView) const
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
