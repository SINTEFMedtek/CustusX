/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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


