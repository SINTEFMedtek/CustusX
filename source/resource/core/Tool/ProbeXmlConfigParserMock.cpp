#include <ProbeXmlConfigParserMock.h>

#include <QStringList>
#include <iostream>

ProbeXmlConfigParserMock::ProbeXmlConfigParserMock(QString& pathToXml) :
	mFileName(pathToXml)
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


