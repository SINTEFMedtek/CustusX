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
	virtual ssc::ProbeData getData() const;
	virtual ssc::VideoSourcePtr getRTSource() const;
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
	void setRTSource(ssc::VideoSourcePtr source);
	ProbeXmlConfigParser::Configuration getConfiguration() const;
	void changeProbeSectorParameters(double depthStart, double depthEnd, double width); ///< Update probe sector parameters
	void changeProbeSectorSize(int width, int height); ///< Update probe sector parameters
	void changeProbeSectorOrigin(ssc::Vector3D origin); ///< Update probe sector origin
	void setProbeSector(ssc::ProbeData probeSector); ///< Set a new probe sector
	void setProbeImageData(ssc::ProbeData::ProbeImageData imageData); ///< Set a new probe image data
	void removeCurrentConfig(); ///< remove the current config from disk
	void saveCurrentConfig(QString uid, QString name); ///< save current config to disk under ids (uid,name).

private:
	Probe(QString instrumentUid, QString scannerUid);
	ProbeXmlConfigParser::Configuration getConfiguration(QString uid) const;
	QString getInstrumentId() const;
	QString getInstrumentScannerId() const;

	ssc::ProbeData mData; ///< Probe sector information
	ssc::VideoSourcePtr mSource;
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

