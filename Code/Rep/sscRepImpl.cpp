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
