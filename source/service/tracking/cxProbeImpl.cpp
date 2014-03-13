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

#include "cxProbeImpl.h"

#include <QStringList>
#include "cxVideoSource.h"
#include "cxMessageManager.h"
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
	retval->setData(this->getProbeData());
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
	this->updateProbeSector();
	this->updateTemporalCalibration();
	this->setSoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
	emit activeConfigChanged();
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
	emit sectorChanged();
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

ProbeXmlConfigParser::Configuration ProbeImpl::getConfiguration() const
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration(this->getConfigId());
	return config;
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
		messageManager()->sendWarning(QString("Found no probe configuration for:\n"
			"scanner=[%1] instrument=[%2].\n"
			"Check that your %3 file contains entries\n"
			"<USScanner> <Name>%1</Name> ... <USProbe> <Name>%2</Name>").arg(mScannerUid).arg(mInstrumentUid).arg(mXml->getFileName()));
	}
}

ProbeXmlConfigParser::Configuration ProbeImpl::getConfiguration(QString uid) const
{
	ProbeXmlConfigParser::Configuration config;
	if(this->hasRtSource())
		config = mXml->getConfiguration(mScannerUid, mInstrumentUid, this->getRtSourceName(), uid);
	return config;
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
	return !this->getConfiguration(this->getConfigId()).isEmpty();
}

ProbeDefinition ProbeImpl::createProbeSector()
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration(this->getConfigId());
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
