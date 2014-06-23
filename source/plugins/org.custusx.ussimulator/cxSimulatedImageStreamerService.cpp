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
#include "cxSimulatedImageStreamerService.h"

#include "cxToolManager.h"
#include "cxDataManager.h"
#include "cxReporter.h"
#include "cxSettings.h"

//These 3 includes should be removed when TrackingManager are a plugin
#include "cxLogicManager.h"
#include "cxVideoService.h"
#include "cxVideoServiceBackend.h"

namespace cx
{

SimulatedImageStreamerService::SimulatedImageStreamerService()
{
}

SimulatedImageStreamerService::~SimulatedImageStreamerService()
{

}

QString SimulatedImageStreamerService::getName()
{
	return "Simulator";
}

void SimulatedImageStreamerService::setImageToStream(QString imageUid)
{
	std::cout << "setting image to " << imageUid.toStdString() << std::endl;
	settings()->setValue("USsimulation/volume", imageUid);
}

void SimulatedImageStreamerService::updateGain()
{
	this->setGain(this->getGainOption(mXmlSettings)->getValue());
}

void SimulatedImageStreamerService::setGain(double gain)
{
	QMutexLocker lock(&mStreamerMutex);
	if(mStreamer)
		mStreamer->setGain(gain/100);
}

std::vector<DataAdapterPtr> SimulatedImageStreamerService::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	retval.push_back(this->getSimulationTypeOption(root));
	retval.push_back(this->getInputImageOption(root));
	retval.push_back(this->getGainOption(root));

	return retval;
}

StreamerPtr SimulatedImageStreamerService::createStreamer(QDomElement root)
{
	QMutexLocker lock(&mStreamerMutex);
	mStreamer.reset(new SimulatedImageStreamer);
	mXmlSettings = root;

	//TODO: remove this dependency when TrackingManager are a plugin
	cx::VideoServiceBackendPtr backend = cx::logicManager()->getVideoService()->getBackend();
	if(!backend)
	{
		reporter()->sendError("SimulatedImageStreamerInterface got no VideoServiceBackend");
		return mStreamer;
	}


	QString selectedVolume = settings()->value("USsimulation/volume", "").toString();
	ToolPtr tool = backend->getToolManager()->findFirstProbe();
	if(!tool)
		reporter()->sendWarning("No tool");
	ImagePtr image = backend->getDataManager()->getImage(selectedVolume);
	if(!image)
		reporter()->sendWarning("No image with uid: "+selectedVolume);

	QString simulationType = this->getSimulationTypeOption(mXmlSettings)->getValue();
	std::cout << "simulationType: " << simulationType.toStdString() << std::endl;
	mStreamer->initialize(image, tool, backend->getDataManager(), simulationType);

	lock.unlock();
	this->updateGain();
	lock.relock();

	return mStreamer;
}

DoubleDataAdapterXmlPtr SimulatedImageStreamerService::getGainOption(QDomElement root)
{
	if(!mSelectedGainDataAdapter)
	{
		mSelectedGainDataAdapter = DoubleDataAdapterXml::initialize("gain", "Gain","Simulates ultrasound scanners gain function.", 70, DoubleRange(1, 100, 1), 0, root);
		connect(mSelectedGainDataAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(updateGain()));
		this->updateGain();
	}

	return mSelectedGainDataAdapter;
}

SelectImageStringDataAdapterPtr SimulatedImageStreamerService::getInputImageOption(QDomElement root)
{
	if(!mSelectImageDataAdapter)
	{
		mSelectImageDataAdapter = SelectImageStringDataAdapter::New();
		QString selectedVolume = settings()->value("USsimulation/volume", "").toString();
		connect(mSelectImageDataAdapter.get(), SIGNAL(dataChanged(QString)), this, SLOT(setImageToStream(QString)));
		mSelectImageDataAdapter->setValue(selectedVolume);
	}

	return mSelectImageDataAdapter;
}

StringDataAdapterXmlPtr SimulatedImageStreamerService::getSimulationTypeOption(QDomElement root)
{
	QString defaultValue = "Original data";
	QStringList simulationTypes;
	simulationTypes << defaultValue << "CT to US";// << "MR to US";

	StringDataAdapterXmlPtr retval;
	retval = StringDataAdapterXml::initialize("simulation_type", "Simulation type", "Simulate us from this kind of image modality", defaultValue, simulationTypes, root);

	return retval;
}

} //end namespace cx
