/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

/*
 * \brief
 *
 *  \date Feb 3, 2011
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */

#ifndef CXPROBEIMPL_H_
#define CXPROBEIMPL_H_

#include "cxResourceExport.h"

#include "cxProbe.h"
#include <map>
#include "ProbeXmlConfigParserImpl.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_tool
 * @{
 */

typedef boost::shared_ptr<class ProbeImpl> ProbeImplPtr;

/**
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT ProbeImpl: public Probe
{
Q_OBJECT
public:
	static ProbeImplPtr New(QString instrumentUid, QString scannerUid, ProbeXmlConfigParserPtr xml = ProbeXmlConfigParserPtr());
	virtual ~ProbeImpl(){}
	virtual bool isValid() const;

	virtual QStringList getAvailableVideoSources();
	virtual VideoSourcePtr getRTSource(QString uid = "active") const;
	virtual ProbeDefinition getProbeDefinition(QString uid = "active") const;
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
	virtual void setProbeDefinition(ProbeDefinition probeDefinition);
	virtual void setRTSource(VideoSourcePtr source);
	virtual void removeRTSource(VideoSourcePtr source);

	virtual void setActiveStream(QString uid);
	virtual QString getActiveStream() const;

	ProbeXmlConfigParser::Configuration getConfiguration();
	virtual void removeCurrentConfig(); ///< remove the current config from disk
	virtual void saveCurrentConfig(QString uid, QString name); ///< save current config to disk under ids (uid,name).

	QString getRtSourceName(QString configurationId = "") const;

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
	std::map<QString, ProbeDefinition> mProbeDefinition; ///< all defined probe definitions
	std::map<QString, VideoSourcePtr> mSource; ///< all defined sources
	ProbeWeakPtr mSelf;

	QString mInstrumentUid;
	QString mScannerUid;
	double mSoundSpeedCompensationFactor;
	bool mOverrideTemporalCalibration;
	double mTemporalCalibration;

	ProbeXmlConfigParserPtr mXml; ///< the xml parser for the ultrasoundImageConfigs.xml
	QString mConfigurationId; ///< The probe sector configuration matching the config id in ultrasoundImageConfigs.xml

	ProbeXmlConfigParser::Configuration mConfig;
	QString findRtSource(QString configId) const;
};

/**
 * @}
 */
} // namespace cx

#endif /* CXPROBEIMPL_H_ */

