#include <ProbeXmlConfigParserMoc.h>

#include <QStringList>
#include <iostream>

ProbeXmlConfigParserMoc::ProbeXmlConfigParserMoc(QString& pathToXml) :
	mFileName(pathToXml)
{}

ProbeXmlConfigParserMoc::~ProbeXmlConfigParserMoc()
{
}

QString ProbeXmlConfigParserMoc::getFileName()
{
	return mFileName;
}

void ProbeXmlConfigParserMoc::removeConfig(QString scanner, QString probe, QString rtsource, QString configId)
{}

void ProbeXmlConfigParserMoc::saveCurrentConfig(ProbeXmlConfigParser::Configuration config)
{}

QStringList ProbeXmlConfigParserMoc::getScannerList()
{
	QStringList retval;
	retval << "DummyScanner";
	return retval;
}

QStringList ProbeXmlConfigParserMoc::getProbeList(QString scanner)
{
	QStringList retval;
	retval << "DummyProbe";
	return retval;
}


QStringList ProbeXmlConfigParserMoc::getRtSourceList(QString scanner, QString probe)
{
	QStringList retval;
	retval << "DummySource";
	return retval;
}

QStringList ProbeXmlConfigParserMoc::getConfigIdList(QString scanner, QString probe, QString rtsource)
{
	QStringList retval;
	QString retString = scanner + " " + probe + " " + rtsource;
	retval << retString;
	return retval;
}

ProbeXmlConfigParser::Configuration ProbeXmlConfigParserMoc::getConfiguration(QString scanner, QString probe, QString rtsource, QString configId)
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


