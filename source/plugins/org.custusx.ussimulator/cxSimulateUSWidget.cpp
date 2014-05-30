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

#include "cxSimulateUSWidget.h"

#include <QVBoxLayout>
#include <QDial>
#include "cxLabeledComboBoxWidget.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxVideoService.h"
#include "cxVideoConnectionManager.h"
#include "cxVideoConnection.h"
#include "cxDataLocations.h"
#include "cxSettings.h"

namespace cx
{
SimulateUSWidget::SimulateUSWidget(QWidget* parent) :
		BaseWidget(parent, "SimulateUSWidget", "Simulated US"),
		mImageSelector(SelectImageStringDataAdapter::New())
{
	QString selectedSimulation = settings()->value("USsimulation/type", "Original data").toString();
	QString selectedVolume = settings()->value("USsimulation/volume", "").toString();
	mImageSelector->setValue(selectedVolume);
	QStringList simulationTypes;
	simulationTypes << "Original data" << "CT to US" << "MR to US";
	mSimulationType = StringDataAdapterXml::initialize("Simulation type", "",
																										 "Simulation run on data extracted from input volume",
																										 selectedSimulation, simulationTypes);
	connect(mSimulationType.get(), SIGNAL(changed()), this, SLOT(simulationTypeChanged()));

	LabeledComboBoxWidget* imageCombo = new LabeledComboBoxWidget(this, mImageSelector);
	connect(mImageSelector.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	this->imageChangedSlot(mImageSelector->getValue());

	mTopLayout = new QVBoxLayout(this);
	mTopLayout->addWidget(new LabeledComboBoxWidget(this, mSimulationType));
	mTopLayout->addWidget(imageCombo);
	this->createAndAddGainController();
}

SimulateUSWidget::~SimulateUSWidget()
{}

void SimulateUSWidget::createAndAddGainController()
{
	double gain = settings()->value("USsimulation/gain").value<double>();
	QDial* mGain = new QDial(this);
	mGain->setMaximum(100);
	mGain->setMinimum(0);
	mGain->setSingleStep(1);
	mGain->setValue(gain*100);
	connect(mGain, SIGNAL(valueChanged(int)), this, SLOT(gainChanged(int)));

	mTopLayout->addWidget(new QLabel("Gain:"));
	mTopLayout->addWidget(mGain, 0, Qt::AlignLeft);
}

QString SimulateUSWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3><Simulation of US based on existing data.</h3>"
			"<p>Lets you set up a connection to a fake streaming server to simulate US recording.</p>"
			"<p><i></i></p>"
			"</html>";
}

QString SimulateUSWidget::getImageUidToSimulate() const
{
	return mImageSelector->getValue();
}

void SimulateUSWidget::setImageUidToSimulate(QString uid)
{
	mImageSelector->setValue(uid);
}

void SimulateUSWidget::simulationTypeChanged()
{
	settings()->setValue("USsimulation/type", mSimulationType->getValue());
}

void SimulateUSWidget::imageChangedSlot(QString imageUid)
{
	if(this->getStreamerInterface())
		mSimulatedStreamerInterface->setImageToStream(imageUid);
	settings()->setValue("USsimulation/volume", mImageSelector->getValue());
}

void SimulateUSWidget::gainChanged(int gain)
{
	if(this->getStreamerInterface())
		mSimulatedStreamerInterface->setGain(gain/100.0);
	settings()->setValue("USsimulation/gain", gain/100.0);
}

SimulatedImageStreamerInterfacePtr SimulateUSWidget::getStreamerInterface()
{
	ImageStreamerInterfacePtr imageStreamerInt = videoService()->getVideoConnection()->getVideoConnection()->getStreamerInterface();
	mSimulatedStreamerInterface = boost::dynamic_pointer_cast<SimulatedImageStreamerService>(imageStreamerInt);
	return mSimulatedStreamerInterface;
}

} /* namespace cx */
