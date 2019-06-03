/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxClippers.h"
#include "cxVisServices.h"
#include "cxLogger.h"
#include "cxInteractiveClipper.h"
#include "cxEnumConversion.h"
#include "cxStringPropertyClipPlane.h"
#include "cxXmlOptionItem.h"
#include "cxProfile.h"
#include "cxStringProperty.h"
#include "cxXmlOptionItem.h"
#include "cxXMLNodeWrapper.h"


namespace cx
{

Clippers::Clippers(VisServicesPtr services) :
	mServices(services)
//	mStorage(services->session(), "Clippers")
{
	this->createDefaultClippers();
//	mStorage.storeVariable("clipperList", boost::bind(&Clippers::exportList, this), boost::bind(&Clippers::importList, this, _1));

//	XmlOptionFile mOptions = profile()->getXmlSettings().descend("clippers");


	//TODO: Store clippers in profile file instead of patient file
//	QString defaultValue = this->getInitialClipperNames().join(';');
//	mClipperList = StringProperty::initialize("clipperList", "Clipper List",
//										"List of clippers",
//										defaultValue, mOptions.getElement());



//	XmlOptionItem xmlStore = XmlOptionItem("clipperList", mOptions.toElement());
//	this->importList(xmlStore.readValue(this->getInitialClipperNames().join(';')));
}

void Clippers::addXml(QDomNode& parentNode)
{
	XMLNodeAdder parent(parentNode);
	XMLNodeAdder clippersNode(parent.addElement("clippers"));

	std::map<QString, InteractiveClipperPtr>::iterator iter = mClippers.begin();
	for (; iter != mClippers.end(); ++iter)
	{
		QDomElement clipperNode = clippersNode.addElement("clipper");
		clipperNode.setAttribute("name", iter->first);
		iter->second->addXml(clipperNode);
	}
}

void Clippers::parseXml(QDomNode parentNode)
{
	XMLNodeParser base(parentNode);

	QDomElement clippersNode = base.parseElement("clippers");
	QDomNode clipperNode = clippersNode.firstChild();
	while (!clipperNode.isNull())
	{
		if (clipperNode.toElement().tagName() != "clipper")
		{
			clipperNode = clipperNode.nextSibling();
			continue;
		}
		QString clipperName = clipperNode.toElement().attribute("name");
		InteractiveClipperPtr clipper = this->getClipper(clipperName);
		clipper->parseXml(clipperNode);

		clipperNode = clipperNode.nextSibling();
	}
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
	else
	{
		InteractiveClipperPtr clipper = InteractiveClipperPtr(new InteractiveClipper(mServices));
		this->add(clipperName, clipper);
		return clipper;
	}
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
