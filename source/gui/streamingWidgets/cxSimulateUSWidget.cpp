#include "cxSimulateUSWidget.h"

#include <QVBoxLayout>
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
}

SimulateUSWidget::~SimulateUSWidget()
{}

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

void SimulateUSWidget::imageChangedSlot(QString imageUid)
{
	videoService()->getVideoConnection()->getVideoConnection()->setImageToStream(imageUid);
	settings()->setValue("USsimulation/volume", mImageSelector->getValue());
}

void SimulateUSWidget::simulationTypeChanged()
{
	settings()->setValue("USsimulation/type", mSimulationType->getValue());
}

} /* namespace cx */
