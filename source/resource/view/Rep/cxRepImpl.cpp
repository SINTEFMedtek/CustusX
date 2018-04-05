/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


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

bool RepImpl::isConnectedToView(ViewPtr theView) const
{
	return this->getView()==theView;
}

void RepImpl::connectToView(ViewPtr theView)
{
	mView = theView;

	vtkRendererPtr renderer = this->getView()->getRenderer();
	renderer->AddObserver(vtkCommand::StartEvent, this->mCallbackCommand, 1.0);

	this->addRepActorsToViewRenderer(theView);
}

void RepImpl::disconnectFromView(ViewPtr theView)
{
	vtkRendererPtr renderer = this->getRenderer();
	if (renderer)
	{
		renderer->RemoveObserver(this->mCallbackCommand);
		this->removeRepActorsFromViewRenderer(theView);
	}
	mView.reset();
}

void RepImpl::printSelf(std::ostream & os, Indent indent)
{
	os << indent << "mUid: " << mUid << std::endl;
	os << indent << "mName: " << mName << std::endl;
	os << indent << "Type: " << getType() << std::endl;
}

ViewPtr RepImpl::getView() const
{
	return mView.lock();
}

vtkRendererPtr RepImpl::getRenderer()
{
	if (!this->getView())
		return vtkRendererPtr();
	return this->getView()->getRenderer();
}


void RepImpl::ProcessEvents(vtkObject* vtkNotUsed(object), unsigned long event, void* clientdata,
		void* vtkNotUsed(calldata))
{
	RepImpl* self = reinterpret_cast<RepImpl*>(clientdata);
	self->onStartRenderPrivate();
	self->onEveryRender();
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
	if (this->getView())
		this->getView()->setModified();
}


} // namespace cx
