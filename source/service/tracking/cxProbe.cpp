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
 * cxProbe.cpp
 *
 *  \date Feb 3, 2011
 *      \author christiana
 */
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

Probe::Probe(QString instrumentUid, QString scannerUid) :
				mSoundSpeedCompensationFactor(1.0),
				mInstrumentUid(instrumentUid),
				mScannerUid(scannerUid)
{
	ssc::ProbeData probeData;
	mData[probeData.getUid()].mData = probeData;
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
	return this->getActiveInternalData().mData.getType() != ssc::ProbeData::tNONE;
}

void Probe::setTemporalCalibration(double val)
{
	mOverrideTemporalCalibration = true;
	mTemporalCalibration = val;
	for (InternalDataType::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
		iter->second.mData.setTemporalCalibration(mTemporalCalibration);
}

void Probe::setSoundSpeedCompensationFactor(double factor)
{
	mSoundSpeedCompensationFactor = factor;
	for (InternalDataType::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
		iter->second.mData.applySoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
}

ssc::ProbeData Probe::getData(QString uid) const
{
	uid = this->toValidUid(uid);
	if (!uid.isEmpty())
		return ssc::ProbeData();
	return mData.find(uid)->second.mData;
}

ssc::VideoSourcePtr Probe::getRTSource(QString uid) const
{
	uid = this->toValidUid(uid);
	if (!uid.isEmpty())
		return ssc::VideoSourcePtr();
	return mData.find(uid)->second.mSource;
}

ProbePtr Probe::New(QString instrumentUid, QString scannerUid)
{
	Probe* object = new Probe(instrumentUid, scannerUid);
	ProbePtr retval(object);
	object->mSelf = retval;
	return retval;
}

void Probe::setRTSource(ssc::VideoSourcePtr source)
{
	SSC_ASSERT(source); // not handled after refactoring - add clear method??
	if (!source)
		return;

	QString uid = this->toValidUid(source->getUid());
	if (uid.isEmpty())
	{
		uid = source->getUid();

		// first erase the default stream if anything else appears
		if (uid!="default" && mData.count("default"))
			mData.erase("default");

		mData[uid];
	}

	StreamData& internalData = mData.find(uid)->second;

	boost::shared_ptr<ssc::ProbeAdapterRTSource> adapter;
	adapter = boost::shared_dynamic_cast<ssc::ProbeAdapterRTSource>(internalData.mSource);
	if (adapter && (source==adapter->getBaseSource()))
		return;

	internalData.mSource.reset();
	if (source)
		adapter.reset(new ssc::ProbeAdapterRTSource(source->getUid() + "_probe", mSelf.lock(), source));	
	internalData.mSource = adapter;

	emit sectorChanged();
}

void Probe::setData(ssc::ProbeData probeSector, QString configUid)
{
	QString uid = this->toValidUid(probeSector.getUid());
	if (uid.isEmpty())
	{
		// add new stream
		uid = probeSector.getUid();

		// first erase the default stream if anything else appears
		if (uid!="default" && mData.count("default"))
			mData.erase("default");

		mData[uid];
	}

	StreamData& internalData = mData.find(uid)->second;

	internalData.mData = probeSector;
	mConfigurationId = configUid;
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
	config = createConfigurationFromProbeData(config, this->getActiveInternalData().mData);

	mXml->saveCurrentConfig(config);
	this->setConfigId(uid);
}

Probe::StreamData& Probe::getActiveInternalData()
{
	SSC_ASSERT(mData.count(mActiveUid));
	return mData.find(mActiveUid)->second;
}
const Probe::StreamData& Probe::getActiveInternalData() const
{
	SSC_ASSERT(mData.count(mActiveUid));
	return mData.find(mActiveUid)->second;
}

Probe::StreamData& Probe::getDataForUid(QString uid)
{
	return mData[uid];
}

QString Probe::toValidUid(QString uid) const
{
	if (uid=="active")
		uid = mActiveUid;
	if (mData.count(uid))
		return uid;
	return "";
}

QStringList Probe::getAvailableVideoSources()
{
	QStringList retval;
	for (InternalDataType::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
		retval << iter->first;
	return retval;
}

void Probe::setActiveStream(QString uid)
{
	uid = this->toValidUid(uid);
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
