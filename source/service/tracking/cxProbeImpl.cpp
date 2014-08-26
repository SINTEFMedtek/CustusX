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

#include "cxProbeImpl.h"

#include <QStringList>
#include "cxVideoSource.h"
#include "cxReporter.h"
#include "cxProbeSector.h"
#include "cxProbeAdapterRTSource.h"
#include "cxTypeConversions.h"
#include "cxVector3D.h"
#include "cxLogger.h"
#include "cxDataLocations.h"
#include "cxCreateProbeDataFromConfiguration.h"

namespace cx
{

ProbeImplPtr ProbeImpl::New(QString instrumentUid, QString scannerUid, ProbeXmlConfigParserPtr xml)
{
	ProbeImpl* object = new ProbeImpl(instrumentUid, scannerUid);
	ProbeImplPtr retval(object);
	object->mSelf = retval;
	retval->initProbeXmlConfigParser(xml);
	retval->initConfigId();
	return retval;
}

bool ProbeImpl::isValid() const
{
	return this->getProbeData("active").getType() != ProbeDefinition::tNONE;
}

QStringList ProbeImpl::getAvailableVideoSources()
{
	QStringList retval;
	for (std::map<QString, VideoSourcePtr>::iterator iter=mSource.begin(); iter!=mSource.end(); ++iter)
		retval << iter->first;
	return retval;
}

VideoSourcePtr ProbeImpl::getRTSource(QString uid) const
{
	if (mSource.empty())
		return VideoSourcePtr();
	if (uid=="active")
		uid = mActiveUid;
	if (mSource.count(uid))
		return mSource.find(uid)->second;
	return mSource.begin()->second;
}

ProbeDefinition ProbeImpl::getProbeData(QString uid) const
{
	ProbeDefinition retval;

	if (uid=="active")
		uid = mActiveUid;
	if (mProbeData.count(uid))
		retval = mProbeData.find(uid)->second;
	else if (mProbeData.count("default"))
		retval = mProbeData.find("default")->second;
	else
		retval = mProbeData.begin()->second;

	// ensure uid is matching the requested uid even if not found.
	retval.setUid(uid);
	return retval;
}

ProbeSectorPtr ProbeImpl::getSector(QString uid)
{
	ProbeSectorPtr retval(new ProbeSector());
	retval->setData(this->getProbeData(uid));
	return retval;
}

void ProbeImpl::addXml(QDomNode& dataNode)
{
	QDomDocument doc = dataNode.ownerDocument();
	dataNode.toElement().setAttribute("config", mConfigurationId);
}

void ProbeImpl::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;
	QString cfg = dataNode.toElement().attribute("config");
	if (cfg.isEmpty())
		return;
	this->applyNewConfigurationWithId(cfg);
}

QStringList ProbeImpl::getConfigIdList() const
{
	if (!this->hasRtSource())
		return QStringList();
	QStringList configIdList = mXml->getConfigIdList(
			this->getInstrumentScannerId(), this->getInstrumentId(), this->getRtSourceName());
	return configIdList;
}

QString ProbeImpl::getConfigName(QString configString) ///< get a name for the given configuration
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration(configString);
	return config.mName;
}

QString ProbeImpl::getConfigId() const
{
	return mConfigurationId;
}

QString ProbeImpl::getConfigurationPath() const
{
	if (!this->hasRtSource())
		return "";
	QStringList retval;
	retval << this->getInstrumentScannerId() << this->getInstrumentId() << this->getRtSourceName() << this->getConfigId();
	return retval.join(":");
}

void ProbeImpl::applyNewConfigurationWithId(QString uid)
{
	this->setConfigId(uid);
	this->applyConfig();
	emit activeConfigChanged();
}

void ProbeImpl::applyConfig()
{
	this->updateProbeSector();
	this->updateTemporalCalibration();
	this->setSoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
}

void ProbeImpl::setTemporalCalibration(double val)
{
	mOverrideTemporalCalibration = true;
	mTemporalCalibration = val;
	for (std::map<QString, ProbeDefinition>::iterator iter=mProbeData.begin(); iter!=mProbeData.end(); ++iter)
		iter->second.setTemporalCalibration(mTemporalCalibration);
}

void ProbeImpl::setSoundSpeedCompensationFactor(double factor)
{
	if(similar(mSoundSpeedCompensationFactor, factor))
		return;
	mSoundSpeedCompensationFactor = factor;
	for (std::map<QString, ProbeDefinition>::iterator iter=mProbeData.begin(); iter!=mProbeData.end(); ++iter)
		iter->second.applySoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
}

void ProbeImpl::setProbeSector(ProbeDefinition probeSector)
{
	if (probeSector.getUid().isEmpty())
		probeSector.setUid(mActiveUid);

	mProbeData[probeSector.getUid()] = probeSector;
	emit sectorChanged();
}

void ProbeImpl::setRTSource(VideoSourcePtr source)
{
	SSC_ASSERT(source); // not handled after refactoring - add clear method??
	if (!source)
		return;

	// uid already exist: check if base object is the same
	if (mSource.count(source->getUid()))
	{
		VideoSourcePtr old = mSource.find(source->getUid())->second;

		boost::shared_ptr<ProbeAdapterRTSource> oldAdapter;
		oldAdapter = boost::dynamic_pointer_cast<ProbeAdapterRTSource>(old);
		// check for identity, ignore if no change
		if (oldAdapter && (source==oldAdapter->getBaseSource()))
			return;
	}

	// must have same uid as original: the uid identifies the video source
	mSource[source->getUid()].reset(new ProbeAdapterRTSource(source->getUid(), mSelf.lock(), source));
	emit sectorChanged();
}

void ProbeImpl::removeRTSource(VideoSourcePtr source)
{
	if (!source)
		return;
	if (!mSource.count(source->getUid()))
		return;

	mSource.erase(source->getUid());
	mProbeData.erase(source->getUid());
	this->applyConfig();//May need to re-create config, as the old ProbeDefinition may be deleted
}

void ProbeImpl::setActiveStream(QString uid)
{
	if (uid.isEmpty())
		return;
	mActiveUid = uid;
	emit sectorChanged();
}

QString ProbeImpl::getActiveStream() const
{
	return mActiveUid;
}

void ProbeImpl::removeCurrentConfig()
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration();

	int index = this->getConfigIdList().indexOf(config.mConfigId);
	if (index<0)
		return;
	if (index!=0)
		--index;

	mXml->removeConfig(config.mUsScanner, config.mUsProbe, config.mRtSource, config.mConfigId);
	if (index < this->getConfigIdList().size())
		this->applyNewConfigurationWithId(this->getConfigIdList()[index]);
	emit sectorChanged();
}

void ProbeImpl::saveCurrentConfig(QString uid, QString name)
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration();
	config.mConfigId = uid;
	config.mName = name;
	config = createConfigurationFromProbeData(config, this->getProbeData("active"));

	mXml->saveCurrentConfig(config);
	this->applyNewConfigurationWithId(uid);
}

void ProbeImpl::useDigitalVideo(bool digitalStatus)
{
	mDigitalInterface = digitalStatus;
	if (mDigitalInterface)
		this->applyNewConfigurationWithId("Digital");
}

bool ProbeImpl::isUsingDigitalVideo() const
{
	return mDigitalInterface;
}

QString ProbeImpl::getRtSourceName() const
{
	QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
	if (rtSourceList.empty())
		return QString();
	QString rtSource = rtSourceList.at(0);
	if (this->isUsingDigitalVideo())
		rtSource = "Digital";
	return rtSource;
}

ProbeImpl::ProbeImpl(QString instrumentUid, QString scannerUid) :
		mInstrumentUid(instrumentUid),
		mScannerUid(scannerUid),
		mSoundSpeedCompensationFactor(1.0),
		mOverrideTemporalCalibration(false),
		mTemporalCalibration(0.0),
		mDigitalInterface(false)
{
	ProbeDefinition probeData;
	mProbeData[probeData.getUid()] = probeData;
	mActiveUid = probeData.getUid();
}

void ProbeImpl::initProbeXmlConfigParser(ProbeXmlConfigParserPtr xml = ProbeXmlConfigParserPtr())
{
	if (!xml)
	{
		QString xmlFileName = cx::DataLocations::getRootConfigPath() + QString("/tool/ProbeCalibConfigs.xml");
		mXml.reset(new ProbeXmlConfigParserImpl(xmlFileName));
	} else
		mXml = xml;
}

void ProbeImpl::initConfigId()
{
	QStringList configs = this->getConfigIdList();
	if (!configs.isEmpty())
		this->applyNewConfigurationWithId(configs[0]);
	else
	{
		reportWarning(QString("Found no probe configuration for:\n"
			"scanner=[%1] instrument=[%2].\n"
			"Check that your %3 file contains entries\n"
			"<USScanner> <Name>%1</Name> ... <USProbe> <Name>%2</Name>").arg(mScannerUid).arg(mInstrumentUid).arg(mXml->getFileName()));
	}
}

ProbeXmlConfigParser::Configuration ProbeImpl::getConfiguration()
{
	if (mConfig.mConfigId != this->getConfigId())
	{
		mConfig = this->getConfiguration(this->getConfigId());
	}
	return mConfig;
}

ProbeXmlConfigParser::Configuration ProbeImpl::getConfiguration(QString uid) const
{
	if (mConfig.mConfigId != uid)
	{
		ProbeXmlConfigParser::Configuration config;
		if(this->hasRtSource())
			config = mXml->getConfiguration(mScannerUid, mInstrumentUid, this->getRtSourceName(), uid);
		return config;
	}
	return mConfig;
}

QString ProbeImpl::getInstrumentId() const
{
	return mInstrumentUid;
}

QString ProbeImpl::getInstrumentScannerId() const
{
	return mScannerUid;
}

bool ProbeImpl::hasRtSource() const
{
	return !(this->getRtSourceName().isEmpty());
}

void ProbeImpl::setConfigId(QString uid)
{
	mConfigurationId = uid;
}

void ProbeImpl::updateProbeSector()
{
	if(this->isValidConfigId() && !this->isUsingDigitalVideo())
	{
		ProbeDefinition probeSector = this->createProbeSector();
		this->setProbeSector(probeSector);
	}
}

bool ProbeImpl::isValidConfigId()
{
	//May need to create ProbeXmlConfigParser::isValidConfig(...) also
	return !this->getConfiguration().isEmpty();
}

ProbeDefinition ProbeImpl::createProbeSector()
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration();
	ProbeDefinition probeSector = createProbeDataFromConfiguration(config);
	probeSector.setUid(mActiveUid);
	return probeSector;
}

void ProbeImpl::updateTemporalCalibration()
{
	if (mOverrideTemporalCalibration)
		this->setTemporalCalibration(mTemporalCalibration);
}


} //namespace cx
