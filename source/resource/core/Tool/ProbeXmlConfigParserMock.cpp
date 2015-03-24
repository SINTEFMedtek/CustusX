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

#include <ProbeXmlConfigParserMock.h>

#include <QStringList>
#include <iostream>

ProbeXmlConfigParserMock::ProbeXmlConfigParserMock(QString& pathToXml, bool provideRTSource) :
	mFileName(pathToXml),
	mProvideRTSource(provideRTSource)
{}

ProbeXmlConfigParserMock::~ProbeXmlConfigParserMock()
{
}

QString ProbeXmlConfigParserMock::getFileName()
{
	return mFileName;
}

void ProbeXmlConfigParserMock::removeConfig(QString scanner, QString probe, QString rtsource, QString configId)
{}

void ProbeXmlConfigParserMock::saveCurrentConfig(ProbeXmlConfigParser::Configuration config)
{}

QStringList ProbeXmlConfigParserMock::getScannerList()
{
	QStringList retval;
	retval << "DummyScanner";
	return retval;
}

QStringList ProbeXmlConfigParserMock::getProbeList(QString scanner)
{
	QStringList retval;
	retval << "DummyProbe";
	return retval;
}


QStringList ProbeXmlConfigParserMock::getRtSourceList(QString scanner, QString probe)
{
	QStringList retval;
	if (mProvideRTSource)
		retval << "DummySource";
	return retval;
}

QStringList ProbeXmlConfigParserMock::getConfigIdList(QString scanner, QString probe, QString rtsource)
{
	QStringList retval;
	QString retString = scanner + " " + probe + " " + rtsource;
	retval << retString;
	return retval;
}

ProbeXmlConfigParser::Configuration ProbeXmlConfigParserMock::getConfiguration(QString scanner, QString probe, QString rtsource, QString configId)
{
	Configuration retval;
	retval.mName = "Dummy config";
	retval.mUsScanner = scanner;
	retval.mUsProbe = probe;
	retval.mRtSource = rtsource;
	retval.mConfigId = configId;
	retval.mNotes = "Found no notes.";
	retval.mTemporalCalibration = 0;
	retval.mEmpty = false;
	return retval;
}


