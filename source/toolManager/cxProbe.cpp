/*
 * cxProbe.cpp
 *
 *  Created on: Feb 3, 2011
 *      Author: christiana
 */
#include "cxProbe.h"

#include <QStringList>
#include "sscRTSource.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxCreateProbeDataFromConfiguration.h"
#include "sscProbeSector.h"
#include "sscProbeAdapterRTSource.h"
#include "sscTypeConversions.h"
#include "sscVector3D.h"

namespace cx
{

Probe::Probe(QString instrumentUid, QString scannerUid) :
    mSoundSpeedCompensationFactor(1.0),
    mInstrumentUid(instrumentUid),
    mScannerUid(scannerUid)
{
  mOverrideTemporalCalibration = false;
  mTemporalCalibration = 0;

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
  return mData.mType!=ssc::ProbeData::tNONE;
}

void Probe::setTemporalCalibration(double val)
{
//  std::cout << "Probe::setTemporalCalibration " << val << std::endl;
  mOverrideTemporalCalibration = true;
  mTemporalCalibration = val;
  this->setConfigId(mConfigurationId);
}

void Probe::setSoundSpeedCompensationFactor(double factor)
{
  mSoundSpeedCompensationFactor = factor;
  this->setConfigId(mConfigurationId);
}

ssc::ProbeData Probe::getData() const
{
  return mData;
}

ssc::RTSourcePtr Probe::getRTSource() const
{
  return mSource;
}

ProbePtr Probe::New(QString instrumentUid, QString scannerUid)
{
  Probe* object = new Probe(instrumentUid, scannerUid);
  ProbePtr retval(object);
  object->mSelf = retval;
  return retval;
}

void Probe::setRTSource(ssc::RTSourcePtr source)
{
  ssc::RTSourcePtr adapter(new ssc::ProbeAdapterRTSource(source->getUid()+"_probe", mSelf.lock(), source));
  mSource = adapter;
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

QStringList Probe::getConfigIdList() const
{
  QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
  if (rtSourceList.empty())
    return QStringList();
  QStringList configIdList = mXml->getConfigIdList(this->getInstrumentScannerId(), this->getInstrumentId(), rtSourceList.at(0));
  return configIdList;
}

QString Probe::getConfigName(QString configString) ///< get a name for the given configuration
{
  ProbeXmlConfigParser::Configuration config = this->getConfiguration(configString);
  return config.mName;
}

QString Probe::getConfigId() const
{
  return mConfigurationId;
}

QString Probe::getConfigurationPath() const
{
  QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
    if(rtSourceList.isEmpty())
      return "";
  QStringList retval;
  retval << this->getInstrumentScannerId() << this->getInstrumentId() << rtSourceList.at(0) << this->getConfigId();
  return retval.join(":");
}

void Probe::setConfigId(QString uid)
{
  ProbeXmlConfigParser::Configuration config = this->getConfiguration(uid);
  if(config.isEmpty())
    return;

  ssc::ProbeData probeSector = createProbeDataFromConfiguration(config);
//  std::cout << "probeSector.mTemporalCalibration" << probeSector.mTemporalCalibration << std::endl;
  mConfigurationId = uid;
  mData = probeSector;
  emit sectorChanged();
}

ProbeXmlConfigParser::Configuration Probe::getConfiguration() const
{
  ProbeXmlConfigParser::Configuration config = this->getConfiguration(this->getConfigId());
  return config;
}



ProbeXmlConfigParser::Configuration Probe::getConfiguration(QString uid) const
{
  ProbeXmlConfigParser::Configuration config;
  QStringList rtSourceList = mXml->getRtSourceList(mScannerUid, mInstrumentUid);
  if(rtSourceList.isEmpty())
    return config;


  config = mXml->getConfiguration(mScannerUid, mInstrumentUid, rtSourceList.at(0), uid);
//  std::cout << "uids " << mScannerUid << " " << mInstrumentUid << " " << rtSourceList.at(0) << " " << uid << std::endl;
//  std::cout << "config.mTemporalCalibration " << config.mTemporalCalibration << std::endl;

  //compensating for different speed of sound in what is scanned by the probe and what is assumed by the scanner
  if(config.mWidthDeg ==  0) //linear probe
  {
    if (!ssc::similar(mSoundSpeedCompensationFactor, 1.0))
    {
      config.mPixelHeight *=mSoundSpeedCompensationFactor;
      //ssc::messageManager()->sendDebug("Modifying configuration for a linear probe with the sound speed compensation factor.");
    }
  }

  if (mOverrideTemporalCalibration)
    config.mTemporalCalibration = mTemporalCalibration;
//  std::cout << "config.mTemporalCalibration " << config.mTemporalCalibration << std::endl;

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
