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

namespace cx
{

Probe::Probe(QString instrumentUid, QString scannerUid) :
				mSoundSpeedCompensationFactor(1.0),
				mInstrumentUid(instrumentUid),
				mScannerUid(scannerUid)
{
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

ssc::ProbeSectorPtr Probe::getSector()
{
	ssc::ProbeSectorPtr retval(new ssc::ProbeSector());
	retval->setData(this->getData());
	return retval;
}

bool Probe::isValid() const
{
	return mData.getType() != ssc::ProbeData::tNONE;
}

void Probe::setTemporalCalibration(double val)
{
//  std::cout << "Probe::setTemporalCalibration " << val << std::endl;
	mOverrideTemporalCalibration = true;
	mTemporalCalibration = val;
	mData.setTemporalCalibration(mTemporalCalibration);
	//this->setConfigId(mConfigurationId);
}

void Probe::setSoundSpeedCompensationFactor(double factor)
{
	mSoundSpeedCompensationFactor = factor;
	mData.applySoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
	//this->setConfigId(mConfigurationId);
}

ssc::ProbeData Probe::getData() const
{
	return mData;
}

ssc::VideoSourcePtr Probe::getRTSource() const
{
	return mSource;
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
	boost::shared_ptr<ssc::ProbeAdapterRTSource> adapter;
	adapter = boost::shared_dynamic_cast<ssc::ProbeAdapterRTSource>(mSource);
	if (adapter && (source==adapter->getBaseSource()))
		return;

	mSource.reset();
	if (source)
		adapter.reset(new ssc::ProbeAdapterRTSource(source->getUid() + "_probe", mSelf.lock(), source));
	mSource = adapter;
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
	mConfigurationId = uid;
	mData = probeSector;
	//Update temporal calibration and sound speed compensation
	if (mOverrideTemporalCalibration)
		this->setTemporalCalibration(mTemporalCalibration);
	this->setSoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
}

void Probe::setData(ssc::ProbeData probeSector, QString configUid)
{
	mData = probeSector;
//	if (!mConfigurationId.endsWith('*'))
//		mConfigurationId += "*";
//	mConfigurationId = "";
	mConfigurationId = configUid;
	emit sectorChanged();
}
//void Probe::setProbeImageData(ssc::ProbeData::ProbeImageData imageData)
//{
//	mData.setImage(imageData);
//	emit sectorChanged();
//}
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
//  std::cout << "uids " << mScannerUid << " " << mInstrumentUid << " " << rtSourceList.at(0) << " " << uid << std::endl;
//  std::cout << "config.mTemporalCalibration " << config.mTemporalCalibration << std::endl;
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

//void Probe::changeProbeSectorParameters(double depthStart, double depthEnd, double width)
//{
////	mData.mDepthStart = depthStart;
////	mData.mDepthEnd = depthEnd;
////	mData.mWidth = width;
//	mData.setSector(depthStart, depthEnd, width);
//	emit sectorChanged();
//}

//void Probe::changeProbeSectorSize(int width, int height)
//{
////	mData.mImage.mSize.setWidth(width);
////	mData.mImage.mSize.setHeight(height);
//	ssc::ProbeData::ProbeImageData image = mData.getImage();
//	image.mSize.setWidth(width);
//	image.mSize.setHeight(height);
//	mData.setImage(image);
//
//	emit sectorChanged();
//}
//void Probe::changeProbeSectorOrigin(ssc::Vector3D origin)
//{
////	mData.mImage.mOrigin_p = origin;
//
//	ssc::ProbeData::ProbeImageData image = mData.getImage();
//	image.mOrigin_p = origin;
//	mData.setImage(image);
//
//	emit sectorChanged();
//}

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
	config = createConfigurationFromProbeData(config, mData);

//TODO: possibly fix old hack on storing temporal calibration??

	mXml->saveCurrentConfig(config);
	this->setConfigId(uid);
}


} //namespace cx
