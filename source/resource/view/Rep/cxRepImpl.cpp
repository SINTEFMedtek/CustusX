/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
