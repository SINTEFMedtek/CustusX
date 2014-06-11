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

/*
 * \brief
 *
 *  \date Feb 3, 2011
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */

#ifndef CXPROBEIMPL_H_
#define CXPROBEIMPL_H_

#include "cxProbe.h"
#include <map>
#include "ProbeXmlConfigParserImpl.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_service_tracking
 * @{
 */

typedef boost::shared_ptr<class ProbeImpl> ProbeImplPtr;

/**
 * \ingroup cx_service_tracking
 */
class ProbeImpl: public Probe
{
Q_OBJECT
public:
	static ProbeImplPtr New(QString instrumentUid, QString scannerUid, ProbeXmlConfigParserPtr xml = ProbeXmlConfigParserPtr());
	virtual ~ProbeImpl(){}
	virtual bool isValid() const;

	virtual QStringList getAvailableVideoSources();
	virtual VideoSourcePtr getRTSource(QString uid = "active") const;
	virtual ProbeDefinition getProbeData(QString uid = "active") const;
	virtual ProbeSectorPtr getSector(QString uid = "active");

	virtual void addXml(QDomNode& dataNode);
	virtual void parseXml(QDomNode& dataNode);

	virtual QStringList getConfigIdList() const;
	virtual QString getConfigName(QString uid);
	virtual QString getConfigId() const;
	virtual QString getConfigurationPath() const;

	virtual void applyNewConfigurationWithId(QString uid);
	virtual void setTemporalCalibration(double val);
	virtual void setSoundSpeedCompensationFactor(double val);
	virtual void setProbeSector(ProbeDefinition probeSector);
	virtual void setRTSource(VideoSourcePtr source);
	virtual void removeRTSource(VideoSourcePtr source);

	virtual void setActiveStream(QString uid);
	virtual QString getActiveStream() const;

	ProbeXmlConfigParser::Configuration getConfiguration() const;
	void removeCurrentConfig(); ///< remove the current config from disk
	void saveCurrentConfig(QString uid, QString name); ///< save current config to disk under ids (uid,name).

	void useDigitalVideo(bool digitalStatus);///< RTSource is digital (eg. US sector is set digitally, not read from .xml file)
	bool isUsingDigitalVideo() const;
	QString getRtSourceName() const;

private:
	ProbeImpl(QString instrumentUid, QString scannerUid);
	void initProbeXmlConfigParser(ProbeXmlConfigParserPtr xml);
	void initConfigId();
	ProbeXmlConfigParser::Configuration getConfiguration(QString uid) const;
	QString getInstrumentId() const;
	QString getInstrumentScannerId() const;
	bool hasRtSource() const;

	void setConfigId(QString uid);
	void updateProbeSector();
	bool isValidConfigId();
	ProbeDefinition createProbeSector();
	void updateTemporalCalibration();
	void applyConfig();

	QString mActiveUid;
	std::map<QString, ProbeDefinition> mProbeData; ///< all defined probe definitions
	std::map<QString, VideoSourcePtr> mSource; ///< all defined sources
	ProbeWeakPtr mSelf;

	QString mInstrumentUid;
	QString mScannerUid;
	double mSoundSpeedCompensationFactor;
	bool mOverrideTemporalCalibration;
	double mTemporalCalibration;
	bool mDigitalInterface;///< RTSource is digital (eg. US sector is set digitally, not read from .xml file)

	ProbeXmlConfigParserPtr mXml; ///< the xml parser for the ultrasoundImageConfigs.xml
	QString mConfigurationId; ///< The probe sector configuration matching the config id in ultrasoundImageConfigs.xml
};

/**
 * @}
 */
} // namespace cx

#endif /* CXPROBEIMPL_H_ */

