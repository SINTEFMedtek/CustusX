/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxProbeImpl.h"

#include <QStringList>
#include "cxVideoSource.h"
#include "cxLogger.h"
#include "cxProbeSector.h"
#include "cxProbeAdapterRTSource.h"
#include "cxTypeConversions.h"
#include "cxVector3D.h"

#include "cxDataLocations.h"
#include "cxCreateProbeDefinitionFromConfiguration.h"

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
	return this->getProbeDefinition("active").getType() != ProbeDefinition::tNONE;
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

ProbeDefinition ProbeImpl::getProbeDefinition(QString uid) const
{
	ProbeDefinition retval;

	if (uid=="active")
		uid = mActiveUid;
	if (mProbeDefinition.count(uid))
		retval = mProbeDefinition.find(uid)->second;
	else if (mProbeDefinition.count("default"))
		retval = mProbeDefinition.find("default")->second;
	else
		retval = mProbeDefinition.begin()->second;

	// ensure uid is matching the requested uid even if not found.
	retval.setUid(uid);
	return retval;
}

ProbeSectorPtr ProbeImpl::getSector(QString uid)
{
	ProbeSectorPtr retval(new ProbeSector());
	retval->setData(this->getProbeDefinition(uid));
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

	// Combine config lists from all RT sources
	QStringList configIdList;
	QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
	for (int i = 0; i < rtSourceList.size(); ++i)
		configIdList << mXml->getConfigIdList(this->getInstrumentScannerId(), this->getInstrumentId(), rtSourceList[i]);

	return configIdList;
}

QString ProbeImpl::getConfigName(QString configString) ///< get a name for the given configuration
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration(configString);
	return config.mName;
}

QString ProbeImpl::getConfigId() const
{
	if (this->getProbeDefinition().getUseDigitalVideo())
		return "Digital";
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
	for (std::map<QString, ProbeDefinition>::iterator iter=mProbeDefinition.begin(); iter!=mProbeDefinition.end(); ++iter)
		iter->second.setTemporalCalibration(mTemporalCalibration);
}

void ProbeImpl::setSoundSpeedCompensationFactor(double factor)
{
	if(similar(mSoundSpeedCompensationFactor, factor))
		return;
	mSoundSpeedCompensationFactor = factor;
	for (std::map<QString, ProbeDefinition>::iterator iter=mProbeDefinition.begin(); iter!=mProbeDefinition.end(); ++iter)
		iter->second.applySoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
}

void ProbeImpl::setProbeDefinition(ProbeDefinition probeDefinition)
{
	if (probeDefinition.getUid().isEmpty())
		probeDefinition.setUid(mActiveUid);

	mProbeDefinition[probeDefinition.getUid()] = probeDefinition;
	emit sectorChanged();
}

void ProbeImpl::setRTSource(VideoSourcePtr source)
{
	CX_ASSERT(source); // not handled after refactoring - add clear method??
	if (!source)
		return;

	// uid already exist: check if base object is the same
	if (mSource.count(source->getUid()))
	{
		VideoSourcePtr old = mSource.find(source->getUid())->second;

		ProbeAdapterRTSourcePtr oldAdapter;
		oldAdapter = boost::dynamic_pointer_cast<ProbeAdapterRTSource>(old);
		// check for identity, ignore if no change
		if (oldAdapter && (source==oldAdapter->getBaseSource()))
			return;
	}

	// must have same uid as original: the uid identifies the video source
	mSource[source->getUid()].reset(new ProbeAdapterRTSource(source->getUid(), mSelf.lock(), source));
	emit sectorChanged();

	emit videoSourceAdded(mSource[source->getUid()]);
}

void ProbeImpl::removeRTSource(VideoSourcePtr source)
{
	if (!source)
		return;
	if (!mSource.count(source->getUid()))
		return;

	mSource.erase(source->getUid());
	mProbeDefinition.erase(source->getUid());
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
	config = createConfigurationFromProbeDefinition(config, this->getProbeDefinition("active"));

	mXml->saveCurrentConfig(config);
	this->applyNewConfigurationWithId(uid);
}

QString ProbeImpl::getRtSourceName(QString configurationId) const
{
	QString configId = configurationId;
	if (configId.isEmpty())
		configId = mConfigurationId;

	return this->findRtSource(configId);
}

QString ProbeImpl::findRtSource(QString configId) const
{
	QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());

	//Use first RT source if no config id
	if(configId.isEmpty() && !rtSourceList.empty())
		return rtSourceList.at(0);

	QString retval;

	for (int i = 0; i < rtSourceList.size(); ++i)
	{
		QStringList configIdList;
		configIdList << mXml->getConfigIdList(this->getInstrumentScannerId(), this->getInstrumentId(), rtSourceList[i]);
		if(configIdList.contains(configId))
		{
			if(!retval.isEmpty())
				reportWarning(QString("Config id is not unique: %1. Scanner %2, probe: %3. Occurring in RT source: %4 and %5").
							  arg(configId).arg(this->getInstrumentScannerId()).arg(this->getInstrumentId()).arg(rtSourceList[i]).arg(retval));
			retval = rtSourceList[i];
		}
	}
    return retval;
}

ProbeImpl::ProbeImpl(QString instrumentUid, QString scannerUid) :
		mInstrumentUid(instrumentUid),
		mScannerUid(scannerUid),
		mSoundSpeedCompensationFactor(1.0),
		mOverrideTemporalCalibration(false),
		mTemporalCalibration(0.0)
{
	ProbeDefinition probeDefinition;
	mProbeDefinition[probeDefinition.getUid()] = probeDefinition;
	mActiveUid = probeDefinition.getUid();
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
			config = mXml->getConfiguration(mScannerUid, mInstrumentUid, this->getRtSourceName(uid), uid);
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
	if(this->isValidConfigId() && !this->getProbeDefinition().getUseDigitalVideo())
	{
		ProbeDefinition probeSector = this->createProbeSector();
		this->setProbeDefinition(probeSector);
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
	ProbeDefinition probeSector = createProbeDefinitionFromConfiguration(config);
	probeSector.setUid(mActiveUid);
	return probeSector;
}

void ProbeImpl::updateTemporalCalibration()
{
	if (mOverrideTemporalCalibration)
		this->setTemporalCalibration(mTemporalCalibration);
}


} //namespace cx
