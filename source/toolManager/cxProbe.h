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
  Q_OBJECT
public:
  static ProbePtr New(QString instrumentUid, QString scannerUid);
  virtual ~Probe() {}
  virtual bool isValid() const;
  virtual ssc::ProbeData getData() const;
  virtual ssc::RTSourcePtr getRTSource() const;
  virtual ssc::ProbeSectorPtr getSector();

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
  void setRTSource(ssc::RTSourcePtr source);
  ProbeXmlConfigParser::Configuration getConfiguration() const;

private:
  Probe(QString instrumentUid, QString scannerUid);
  ProbeXmlConfigParser::Configuration getConfiguration(QString uid) const;
  QString getInstrumentId() const;
  QString getInstrumentScannerId() const;

  ssc::ProbeData mData; ///< Probe sector information
  ssc::RTSourcePtr mSource;
  ssc::ProbeWeakPtr mSelf;

  double mSoundSpeedCompensationFactor;
  bool mOverrideTemporalCalibration;
  double mTemporalCalibration;

  QString mInstrumentUid;
  QString mScannerUid;
  boost::shared_ptr<ProbeXmlConfigParser> mXml; ///< the xml parser for the ultrasoundImageConfigs.xml
  QString mConfigurationId; ///< The probe sector configuration matching the config id in ultrasoundImageConfigs.xml

};

} // namespace cx


#endif /* CXPROBE_H_ */


