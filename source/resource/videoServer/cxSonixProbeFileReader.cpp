// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#include "cxSonixProbeFileReader.h"
#include <iostream>
#include <QFile>
#include <QDomDocument>
#include "cxTypeConversions.h"

namespace cx {

SonixProbeFileReader::SonixProbeFileReader(QString probeFile) :
	mFile(probeFile)
{
}

bool SonixProbeFileReader::init()
{
	if(!this->openFile())
		return false;

	if(!this->openDomDocument())
		return false;

	if(!this->openDocumentElement())
		return false;

	return true;
}

bool SonixProbeFileReader::openFile()
{
	if(!mFile.open(QIODevice::ReadOnly))
		return false;

	return true;
}

bool SonixProbeFileReader::openDomDocument()
{
	if(!mDoc.setContent(&mFile))
	{
		mFile.close();
		return false;
	}
	return true;
}

bool SonixProbeFileReader::openDocumentElement()
{
	mDocElem = mDoc.documentElement();
	if(mDocElem.isNull())
		return false;

	return true;
}

QDomNode SonixProbeFileReader::getProbeNode(QString probeName)
{
	QDomElement probes = this->getProbes();
	QDomNode probeNode = probes.firstChild();
	QDomElement retval;
	while(!probeNode.isNull() && retval.isNull())
	{
		retval = probeNode.toElement();
		if(!retval.isNull())
		{
			if(retval.attribute("name") != probeName)
			{
				retval = QDomElement();
				probeNode = probeNode.nextSibling();
			}
		}
	}
	return retval;
}

QDomElement SonixProbeFileReader::getProbes()
{
	return this->getChildWithTag(mDocElem, "probes");
}

bool SonixProbeFileReader::isProbeLinear(QDomNode probeNode)
{
	QDomElement type = this->getChildWithTag(probeNode, "type");
	if (type.text() == "2")
		return true;

	return false;
}

QDomElement SonixProbeFileReader::getChildWithTag(QDomNode parent, QString tagName)
{
	QDomNode probesNode = parent.firstChild();
	QDomElement retval;
	while(!probesNode.isNull() && retval.isNull())
	{
		retval = probesNode.toElement();
		if(!retval.isNull())
		{
			if(retval.tagName() != tagName)
			{
				retval = QDomElement();
				probesNode = probesNode.nextSibling();
			}
		}
	}
	return retval;
}

long SonixProbeFileReader::getProbeLenght(QDomNode probeNode)
{
	long retval = this->getProbeParam(probeNode, "pitch") * this->getProbeParam(probeNode, "numElements");
	return retval;
}

long SonixProbeFileReader::getProbeParam(QDomNode probeNode, QString param)
{
	QDomElement element = this->getChildWithTag(probeNode, param);
	QString val = element.text();
	return val.toLong();
}

} //namespace cx
