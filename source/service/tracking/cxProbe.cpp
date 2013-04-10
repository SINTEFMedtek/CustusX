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

#include "cxProbe.h"

#include <QStringList>
#include "sscVideoSource.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxCreateProbeDataFromConfiguration.h"
#include "sscProbeSector.h"
#include "sscProbeAdapterRTSource.h"
#include "sscTypeConversions.h"
#include "sscVector3D.h"
#include "sscLogger.h"

namespace cx
{

ProbePtr Probe::New(QString instrumentUid, QString scannerUid)
{
	Probe* object = new Probe(instrumentUid, scannerUid);
	ProbePtr retval(object);
	object->mSelf = retval;
	return retval;
}

Probe::Probe(QString instrumentUid, QString scannerUid) :
				mSoundSpeedCompensationFactor(1.0),
				mInstrumentUid(instrumentUid),
				mScannerUid(scannerUid)
{
	ssc::ProbeData probeData;
	mProbeData[probeData.getUid()] = probeData;
	mActiveUid = probeData.getUid();

	mOverrideTemporalCalibration = false;
	mTemporalCalibration = 0;
	QString xmlFileName = cx::DataLocations::getRootConfigPath() + QString("/tool/ProbeCalibConfigs.xml");
	mXml.reset(new ProbeXmlConfigParser(xmlFileName));

	QStringList configs = this->getConfigIdList();
	if (!configs.isEmpty())
		this->setConfigId(configs[0]);
	else
	{
		ssc::messageManager()->sendWarning(QString("Found no probe configuration for:\n"
			"scanner=[%1] instrument=[%2].\n"
			"Check that your %3 file contains entries\n"
			"<USScanner> <Name>%1</Name> ... <USProbe> <Name>%2</Name>").arg(scannerUid).arg(instrumentUid).arg(xmlFileName));
	}
}

ssc::ProbeSectorPtr Probe::getSector(QString uid)
{
	ssc::ProbeSectorPtr retval(new ssc::ProbeSector());
	retval->setData(this->getData());
	return retval;
}

bool Probe::isValid() const
{
	return this->getProbeData("active").getType() != ssc::ProbeData::tNONE;
}

ssc::ProbeData Probe::getProbeData(QString uid) const
{
	ssc::ProbeData retval;

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

void Probe::setTemporalCalibration(double val)
{
	mOverrideTemporalCalibration = true;
	mTemporalCalibration = val;
	for (std::map<QString, ssc::ProbeData>::iterator iter=mProbeData.begin(); iter!=mProbeData.end(); ++iter)
		iter->second.setTemporalCalibration(mTemporalCalibration);
}

void Probe::setSoundSpeedCompensationFactor(double factor)
{
	mSoundSpeedCompensationFactor = factor;
	for (std::map<QString, ssc::ProbeData>::iterator iter=mProbeData.begin(); iter!=mProbeData.end(); ++iter)
		iter->second.applySoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
}

ssc::ProbeData Probe::getData(QString uid) const
{
	ssc::ProbeData retval = this->getProbeData(uid);
	return retval;
}

ssc::VideoSourcePtr Probe::getRTSource(QString uid) const
{
	if (mSource.empty())
		return ssc::VideoSourcePtr();
	if (uid=="active")
		uid = mActiveUid;
	if (mSource.count(uid))
		return mSource.find(uid)->second;
	return mSource.begin()->second;
}

void Probe::setRTSource(ssc::VideoSourcePtr source)
{
	SSC_ASSERT(source); // not handled after refactoring - add clear method??
	if (!source)
		return;

	// uid already exist: check if base object is the same
	if (mSource.count(source->getUid()))
	{
		ssc::VideoSourcePtr old = mSource.find(source->getUid())->second;

		boost::shared_ptr<ssc::ProbeAdapterRTSource> oldAdapter;
		oldAdapter = boost::dynamic_pointer_cast<ssc::ProbeAdapterRTSource>(old);
		// check for identity, ignore if no change
		if (oldAdapter && (source==oldAdapter->getBaseSource()))
			return;
	}

	// must have same uid as original: the uid identifies the video source
	mSource[source->getUid()].reset(new ssc::ProbeAdapterRTSource(source->getUid(), mSelf.lock(), source));
	emit sectorChanged();
}

void Probe::removeRTSource(ssc::VideoSourcePtr source)
{
	if (!source)
		return;
	if (!mSource.count(source->getUid()))
		return;

	mSource.erase(source->getUid());
	mProbeData.erase(source->getUid());
	emit sectorChanged();
}

void Probe::setData(ssc::ProbeData probeSector, QString configUid)
{
	if (probeSector.getUid().isEmpty())
		probeSector.setUid(mActiveUid);

	mProbeData[probeSector.getUid()] = probeSector;
	mConfigurationId = configUid;

//	std::cout << "Probe::setData \n" << streamXml2String(probeSector) << std::endl;

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
	QStringList configIdList = mXml->getConfigIdList(this->getInstrumentScannerId(), this->getInstrumentId(),
					rtSourceList.at(0));
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
	if (rtSourceList.isEmpty())
		return "";
	QStringList retval;
	retval << this->getInstrumentScannerId() << this->getInstrumentId() << rtSourceList.at(0) << this->getConfigId();
	return retval.join(":");
}

void Probe::setConfigId(QString uid)
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration(uid);
	if (config.isEmpty())
		return;

	ssc::ProbeData probeSector = createProbeDataFromConfiguration(config);
//  std::cout << "probeSector.mTemporalCalibration" << probeSector.mTemporalCalibration << std::endl;
//	mConfigurationId = uid;
//	mData = probeSector;
	probeSector.setUid(mActiveUid);
	this->setData(probeSector, uid);
	//Update temporal calibration and sound speed compensation
	if (mOverrideTemporalCalibration)
		this->setTemporalCalibration(mTemporalCalibration);
	this->setSoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
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
	if (rtSourceList.isEmpty())
		return config;

	config = mXml->getConfiguration(mScannerUid, mInstrumentUid, rtSourceList.at(0), uid);
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

void Probe::removeCurrentConfig()
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration();

	int index = this->getConfigIdList().indexOf(config.mConfigId);
	if (index<0)
		return;
	if (index!=0)
		--index;

	mXml->removeConfig(config.mUsScanner, config.mUsProbe, config.mRtSource, config.mConfigId);
	if (index < this->getConfigIdList().size())
		this->setConfigId(this->getConfigIdList()[index]);
	emit sectorChanged();
}

void Probe::saveCurrentConfig(QString uid, QString name)
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration();
	config.mConfigId = uid;
	config.mName = name;
	config = createConfigurationFromProbeData(config, this->getProbeData("active"));

	mXml->saveCurrentConfig(config);
	this->setConfigId(uid);
}

QStringList Probe::getAvailableVideoSources()
{
	QStringList retval;
	for (std::map<QString, ssc::VideoSourcePtr>::iterator iter=mSource.begin(); iter!=mSource.end(); ++iter)
		retval << iter->first;
//	std::cout << "Probe::getAvailableVideoSources " << retval.size() << std::endl;
	return retval;
}

void Probe::setActiveStream(QString uid)
{
	if (uid.isEmpty())
		return;
	mActiveUid = uid;
	emit sectorChanged();
}

QString Probe::getActiveStream() const
{
	return mActiveUid;
}


} //namespace cx
