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

#include "cxRepImpl.h"
#include "cxTypeConversions.h"
#include "cxView.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderer.h"

namespace cx
{

RepImpl::RepImpl(const QString& uid, const QString& name) :
	mName(name), mUid(uid)
{
	mView = NULL;

	mModified = true;
	this->mCallbackCommand = vtkCallbackCommandPtr::New();
	this->mCallbackCommand->SetClientData(this);
	this->mCallbackCommand->SetCallback(RepImpl::ProcessEvents);
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
	return mView==theView;
}

void RepImpl::connectToView(View *theView)
{
	mView = theView;

	vtkRendererPtr renderer = mView->getRenderer();
	renderer->AddObserver(vtkCommand::StartEvent, this->mCallbackCommand, 1.0);

	this->addRepActorsToViewRenderer(theView);
}

void RepImpl::disconnectFromView(View *theView)
{
//	mViews.erase(theView);
	vtkRendererPtr renderer = mView->getRenderer();
	renderer->RemoveObserver(this->mCallbackCommand);

	this->removeRepActorsFromViewRenderer(theView);
	mView = NULL;
}

void RepImpl::printSelf(std::ostream & os, Indent indent)
{
	os << indent << "mUid: " << mUid << std::endl;
	os << indent << "mName: " << mName << std::endl;
	os << indent << "Type: " << getType() << std::endl;
}

View* RepImpl::getView()
{
	return mView;
}

vtkRendererPtr RepImpl::getRenderer()
{
	if (!mView)
		return vtkRendererPtr();
	return this->getView()->getRenderer();
}


void RepImpl::ProcessEvents(vtkObject* vtkNotUsed(object), unsigned long event, void* clientdata,
		void* vtkNotUsed(calldata))
{
	RepImpl* self = reinterpret_cast<RepImpl*>(clientdata);
	self->onStartRenderPrivate();
}

void RepImpl::onStartRenderPrivate()
{
	if (!mModified)
		return;
	this->onModifiedStartRender();
	mModified = false;
}

void RepImpl::setModified()
{
	mModified = true;
}


} // namespace cx
