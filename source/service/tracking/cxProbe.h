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
 * cxProbe.h
 *
 *  \date Feb 3, 2011
 *      \author christiana
 */

#ifndef CXPROBE_H_
#define CXPROBE_H_

#include "sscTool.h"
#include "probeXmlConfigParser.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceTracking
 * @{
 */

typedef boost::shared_ptr<class Probe> ProbePtr;

/**
 * \ingroup cxServiceTracking
 */
class Probe: public ssc::Probe
{
Q_OBJECT
public:
	static ProbePtr New(QString instrumentUid, QString scannerUid);
	virtual ~Probe()
	{
	}
	virtual bool isValid() const;

	virtual QStringList getAvailableVideoSources();
	virtual ssc::VideoSourcePtr getRTSource(QString uid = "active") const;
	virtual ssc::ProbeData getData(QString uid = "active") const;
	virtual ssc::ProbeSectorPtr getSector(QString uid = "active");

	virtual void addXml(QDomNode& dataNode);
	virtual void parseXml(QDomNode& dataNode);

	virtual QStringList getConfigIdList() const;
	virtual QString getConfigName(QString uid);
	virtual QString getConfigId() const;
	virtual QString getConfigurationPath() const;

	virtual void setConfigId(QString uid);
	virtual void setTemporalCalibration(double val);
	virtual void setSoundSpeedCompensationFactor(double val);
	virtual void setData(ssc::ProbeData probeSector, QString configUid="");
	virtual void setRTSource(ssc::VideoSourcePtr source);

	virtual void setActiveStream(QString uid);
	virtual QString getActiveStream() const;

	// non-inherited methods
	ProbeXmlConfigParser::Configuration getConfiguration() const;
	void removeCurrentConfig(); ///< remove the current config from disk
	void saveCurrentConfig(QString uid, QString name); ///< save current config to disk under ids (uid,name).

private:
	Probe(QString instrumentUid, QString scannerUid);
	ProbeXmlConfigParser::Configuration getConfiguration(QString uid) const;
	QString getInstrumentId() const;
	QString getInstrumentScannerId() const;
	ssc::ProbeData getProbeData(QString uid) const;

//	struct StreamData
//	{
//		ssc::ProbeData mData; ///< Probe sector information
//		ssc::VideoSourcePtr mSource;
//	};
	QString mActiveUid;
//	const StreamData& getActiveInternalData() const;
//	StreamData& getActiveInternalData();

//	/** return a reference to the internal StreamData
//	  * object for uid.
//	  * If uid=="active", the mActiveUid is used.
//	  * If uid does not exist, an entry is created.
//	  */
//	StreamData& getDataForUid(QString uid);
//	/** Convert input uid to a valid uid,
//	  * or empty if no valid uid can be found.
//	  */
//	QString toValidUid(QString uid) const;

	std::map<QString, ssc::ProbeData> mProbeData; ///< all defined probe definitions
	std::map<QString, ssc::VideoSourcePtr> mSource; ///< all defined sources
//	typedef std::map<QString, StreamData> InternalDataType;
//	InternalDataType mData;
//	ssc::ProbeData mData; ///< Probe sector information
//	ssc::VideoSourcePtr mSource;
	ssc::ProbeWeakPtr mSelf;

	double mSoundSpeedCompensationFactor;
	bool mOverrideTemporalCalibration;
	double mTemporalCalibration;

	QString mInstrumentUid;
	QString mScannerUid;
	boost::shared_ptr<ProbeXmlConfigParser> mXml; ///< the xml parser for the ultrasoundImageConfigs.xml
	QString mConfigurationId; ///< The probe sector configuration matching the config id in ultrasoundImageConfigs.xml

};

/**
 * @}
 */
} // namespace cx

#endif /* CXPROBE_H_ */

