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

#include "cxClippers.h"
#include "cxVisServices.h"
#include "cxLogger.h"
#include "cxInteractiveClipper.h"
#include "cxEnumConverter.h"
#include "cxStringPropertyClipPlane.h"
#include "cxXmlOptionItem.h"
#include "cxProfile.h"
#include "cxStringProperty.h"
#include "cxXmlOptionItem.h"


namespace cx
{

Clippers::Clippers(VisServicesPtr services) :
	mServices(services)
//	mStorage(services->session(), "Clippers")
{
	this->createDefaultClippers();
//	mStorage.storeVariable("clipperList", boost::bind(&Clippers::exportList, this), boost::bind(&Clippers::importList, this, _1));

	XmlOptionFile mOptions = profile()->getXmlSettings().descend("clippers");


	//TODO: Store clippers in profile file instead of patient file
//	QString defaultValue = this->getInitialClipperNames().join(';');
//	mClipperList = StringProperty::initialize("clipperList", "Clipper List",
//										"List of clippers",
//										defaultValue, mOptions.getElement());



//	XmlOptionItem xmlStore = XmlOptionItem("clipperList", mOptions.toElement());
//	this->importList(xmlStore.readValue(this->getInitialClipperNames().join(';')));
}

void Clippers::importList(QString clippers)
{
	mClipperList.clear();
	if(!clippers.isEmpty())
		mClipperList = clippers.split(';');

	emit changed();
}

QString Clippers::exportList()
{
	return mClipperList.join(';');
}

void Clippers::createDefaultClippers()
{
	QStringList initialList = this->getInitialClipperNames();

	foreach(QString name, initialList)
	{
		InteractiveClipperPtr interactiveClipper = InteractiveClipperPtr(new InteractiveClipper(mServices));

		PLANE_TYPE plane = string2enum<PLANE_TYPE> (name);
		interactiveClipper->setSlicePlane(plane);
		this->add(name, interactiveClipper);
	}
}

QStringList Clippers::getInitialClipperNames()
{
	InteractiveClipperPtr interactiveClipper = InteractiveClipperPtr(new InteractiveClipper(mServices));
	StringPropertyBasePtr planeAdapter = StringPropertyClipPlane::New(interactiveClipper);
	QStringList clipperNames = planeAdapter->getValueRange();
	return clipperNames;
}

InteractiveClipperPtr Clippers::getClipper(QString clipperName)
{
	if(this->exists(clipperName))
		return mClippers.at(clipperName);
	return InteractiveClipperPtr();
}

void Clippers::add(QString clipperName, InteractiveClipperPtr clipper)
{
	if(!clipper)
		return;
	if(!this->exists(clipperName))
	{
		mClippers[clipperName] = clipper;
		mClipperList << clipperName;
		connect(clipper.get(), &InteractiveClipper::changed, this, &Clippers::changed);
	}
	else
		CX_LOG_WARNING() << "Cannot add clipper: " << clipperName << " already exists";
}

void Clippers::remove(QString clipperName)
{
	if(this->exists(clipperName))
	{
		InteractiveClipperPtr clipper = mClippers[clipperName];
		disconnect(clipper.get(), &InteractiveClipper::changed, this, &Clippers::changed);
	}

	mClippers.erase(clipperName);
	int index = mClipperList.indexOf(clipperName);
	if(index >= 0)
		mClipperList.removeAt(index);
	emit changed();
}

bool Clippers::exists(QString clipperName)
{
	return mClippers.find(clipperName) != mClippers.end();
}

QStringList Clippers::getClipperNames()
{
	return mClipperList;
}



}//cx
