/*
 * cxProbe.cpp
 *
 *  Created on: Feb 3, 2011
 *      Author: christiana
 */
#include "cxProbe.h"

#include <QStringList>
#include "sscRealTimeStreamSource.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxCreateProbeDataFromConfiguration.h"
#include "sscProbeSector.h"


namespace cx
{

Probe::Probe(QString instrumentUid, QString scannerUid) :
    mSoundSpeedCompensationFactor(1.0),
    mInstrumentUid(instrumentUid),
    mScannerUid(scannerUid)
{
  // Read ultrasoundImageConfigs.xml file
  QString xmlFileName = cx::DataLocations::getRootConfigPath()+QString("/tool/ProbeCalibConfigs.xml");
  mXml.reset(new ProbeXmlConfigParser(xmlFileName));

  QStringList configs = this->getConfigIdList();
  if (!configs.isEmpty())
    this->setConfigId(configs[0]);
}

ssc::ProbeSectorPtr Probe::getSector()
{
  ssc::ProbeSectorPtr retval(new ssc::ProbeSector());
  retval->setData(this->getData());
  return retval;
}

bool Probe::isValid() const
{
  return mSector.mType!=ssc::ProbeData::tNONE;
}

void Probe::setTemporalCalibration(double val)
{
  mSector.mTemporalCalibration = val;
  emit sectorChanged();
}

void Probe::setSoundSpeedCompensationFactor(double val)
{
  mSector.mSoundSpeedCompensationFactor = val;
  emit sectorChanged();
}

ssc::ProbeData Probe::getData() const
{
  return mSector;
}

ssc::RealTimeStreamSourcePtr Probe::getRealTimeStreamSource() const
{
  return mSource;
}

void Probe::setRealTimeStreamSource(ssc::RealTimeStreamSourcePtr source)
{
  mSource =  source;
  emit sectorChanged();
}

void Probe::addXml(QDomNode& dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();
  dataNode.toElement().setAttribute("config", mConfigurationId);
}

void Probe::parseXml(QDomNode& dataNode)
{
  if (dataNode.isNull())
    return;
  QString cfg = dataNode.toElement().attribute("config");
  if (cfg.isEmpty())
    return;
  this->setConfigId(cfg);
}

//QStringList Probe::getUSSectorConfigList() const
QStringList Probe::getConfigIdList() const
{
  QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
  if (rtSourceList.empty())
    return QStringList();
  QStringList configIdList = mXml->getConfigIdList(this->getInstrumentScannerId(), this->getInstrumentId(), rtSourceList.at(0));
  return configIdList;
}

//QString Probe::getNameOfProbeSectorConfigId(QString configString) ///< get a name for the given configuration
QString Probe::getConfigName(QString configString) ///< get a name for the given configuration
{
  ProbeXmlConfigParser::Configuration config = this->getConfiguration(configString);
  return config.mName;
}

//QString Probe::getProbeSectorConfigIdString() const
QString Probe::getConfigId() const
{
  return mConfigurationId;
}

//QString Probe::getConfigurationString() const
QString Probe::getConfigurationPath() const
{
  QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
    if(rtSourceList.isEmpty())
      return "";
  QStringList retval;
  retval << this->getInstrumentScannerId() << this->getInstrumentId() << rtSourceList.at(0) << this->getConfigId();
  return retval.join(":");
}

//void Probe::setProbeSectorConfigIdString(QString configString)
void Probe::setConfigId(QString uid)
{
  ProbeXmlConfigParser::Configuration config = this->getConfiguration(uid);
  if(config.isEmpty())
    return;

  ssc::ProbeData probeSector = createProbeDataFromConfiguration(config);
  mConfigurationId = uid;
  this->setUSProbeSector(probeSector);
}


void Probe::setUSProbeSector(ssc::ProbeData probeSector)
{
  mSector = probeSector;
  emit sectorChanged();
}


ProbeXmlConfigParser::Configuration Probe::getConfiguration() const
{
  ProbeXmlConfigParser::Configuration config = this->getConfiguration(this->getConfigId());
  return config;
}

void Probe::setSoundSpeedCompensationFactor(double factor)
{
  mSoundSpeedCompensationFactor = factor;
  emit sectorChanged();
}

ProbeXmlConfigParser::Configuration Probe::getConfiguration(QString uid) const
{
  ProbeXmlConfigParser::Configuration config;
  QStringList rtSourceList = mXml->getRtSourceList(mScannerUid, mInstrumentUid);
  if(rtSourceList.isEmpty())
    return config;

  config = mXml->getConfiguration(mScannerUid, mInstrumentUid, rtSourceList.at(0), uid);

  //compensating for different speed of sound in what is scanned by the probe and what is assumed by the scanner
  if(config.mWidthDeg ==  0) //linear probe
  {
    config.mPixelHeight *=mSoundSpeedCompensationFactor;
    ssc::messageManager()->sendDebug("Modifying configuration for a linear probe with the sound speed comensation factor.");
  }

  return config;
}

QString Probe::getInstrumentId() const
{
  return mInstrumentUid;
}
QString Probe::getInstrumentScannerId() const
{
  return mScannerUid;
}



} //namespace cx
