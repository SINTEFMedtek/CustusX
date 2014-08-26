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
