/*
 * cxProbe.h
 *
 *  Created on: Feb 3, 2011
 *      Author: christiana
 */

#ifndef CXPROBE_H_
#define CXPROBE_H_

#include "sscTool.h"
#include "probeXmlConfigParser.h"

namespace cx
{
typedef boost::shared_ptr<class Probe> ProbePtr;

class Probe : public ssc::Probe
{
public:
  virtual ~Probe() {}
  virtual bool isValid() const;
  virtual ssc::ProbeData getData() const;
  virtual ssc::RealTimeStreamSourcePtr getRealTimeStreamSource() const;
  virtual ssc::ProbeSectorPtr getSector();

  Probe(QString instrumentUid, QString scannerUid);

  virtual void addXml(QDomNode& dataNode);
  virtual void parseXml(QDomNode& dataNode);

  virtual QStringList getConfigIdList() const;
  virtual QString getConfigName(QString uid);
  virtual QString getConfigId() const;
  virtual QString getConfigurationPath() const;

  virtual void setConfigId(QString uid);
  virtual void setTemporalCalibration(double val);
  virtual void setSoundSpeedCompensationFactor(double val);

  // non-inherited methods
  void setRealTimeStreamSource(ssc::RealTimeStreamSourcePtr source);
  ProbeXmlConfigParser::Configuration getConfiguration() const;
  void setSoundSpeedCompensationFactor(double factor);

private:
  void setUSProbeSector(ssc::ProbeData probeSector);
  ProbeXmlConfigParser::Configuration getConfiguration(QString uid) const;
  QString getInstrumentId() const;
  QString getInstrumentScannerId() const;

  ssc::ProbeData mSector; ///< Probe sector information
  ssc::RealTimeStreamSourcePtr mSource;

  double mSoundSpeedCompensationFactor;

  QString mInstrumentUid;
  QString mScannerUid;
  boost::shared_ptr<ProbeXmlConfigParser> mXml; ///< the xml parser for the ultrasoundImageConfigs.xml
  QString mConfigurationId; ///< The probe sector configuration matching the config id in ultrasoundImageConfigs.xml

};

} // namespace cx


#endif /* CXPROBE_H_ */


