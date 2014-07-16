#include "cxUSAcqusitionWidget.h"

#include <QtGui>
#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "cxToolManager.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDoubleWidgets.h"
#include "cxTypeConversions.h"
#include "cxPatientData.h"
#include "cxSoundSpeedConversionWidget.h"
#include "cxRecordSessionWidget.h"
#include "cxSettings.h"
#include "cxToolDataAdapters.h"
#include "cxDoubleDataAdapterTemporalCalibration.h"
#include "cxReconstructionManager.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxProbeConfigWidget.h"
#include "cxDisplayTimerWidget.h"
#include "cxReconstructParams.h"
#include "cxTimedAlgorithm.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxStringDataAdapterXml.h"

namespace cx
{

USAcqusitionWidget::USAcqusitionWidget(AcquisitionDataPtr pluginData, QWidget* parent) :
	RecordBaseWidget(pluginData, parent, settings()->value("Ultrasound/acquisitionName").toString())
{
	this->setObjectName("USAcqusitionWidget");
	this->setWindowTitle("US Acquisition");

	connect(mPluginData->getReconstructer().get(), SIGNAL(reconstructAboutToStart()), this, SLOT(reconstructAboutToStartSlot()));
	connect(mPluginData->getReconstructer().get(), SIGNAL(reconstructStarted()), this, SLOT(reconstructStartedSlot()));
	connect(mPluginData->getReconstructer().get(), SIGNAL(reconstructFinished()), this, SLOT(reconstructFinishedSlot()));

	mAcquisition.reset(new USAcquisition(mBase));
	connect(mAcquisition.get(), SIGNAL(acquisitionDataReady()), this, SLOT(acquisitionDataReadySlot()));

	connect(mBase.get(), SIGNAL(stateChanged()), this, SLOT(acquisitionStateChangedSlot()));
	connect(mBase.get(), SIGNAL(started()), this, SLOT(recordStarted()));
	connect(mBase.get(), SIGNAL(acquisitionStopped()), this, SLOT(recordStopped()), Qt::DirectConnection);
	connect(mBase.get(), SIGNAL(cancelled()), this, SLOT(recordCancelled()));
	connect(mAcquisition.get(), SIGNAL(saveDataCompleted(QString)), mPluginData->getReconstructer().get(), SIGNAL(newInputDataAvailable(QString)));

	mRecordSessionWidget->setDescriptionVisibility(false);

	QHBoxLayout* timerLayout = new QHBoxLayout;
	mLayout->addLayout(timerLayout);
	mDisplayTimerWidget = new DisplayTimerWidget(this);
	mDisplayTimerWidget ->setFontSize(10);
	timerLayout->addStretch();
	timerLayout->addWidget(mDisplayTimerWidget);
	timerLayout->addStretch();

	QGridLayout* editsLayout = new QGridLayout;
	editsLayout->setColumnStretch(0,0);
	editsLayout->setColumnStretch(1,1);
	RecordBaseWidget::mLayout->addLayout(editsLayout);
	new LabeledComboBoxWidget(this, ActiveProbeConfigurationStringDataAdapter::New(), editsLayout, 0);
	new LabeledComboBoxWidget(this, mPluginData->getReconstructer()->getParams()->mPresetTFAdapter, editsLayout, 1);

	QAction* optionsAction = this->createAction(this,
	      QIcon(":/icons/open_icon_library/system-run-5.png"),
	      "Details", "Show Details",
	      SLOT(toggleDetailsSlot()),
	      NULL);

	QToolButton* optionsButton = new QToolButton();
	optionsButton->setDefaultAction(optionsAction);
	editsLayout->addWidget(optionsButton, 0, 2);

	mOptionsWidget = this->createOptionsWidget();
	mOptionsWidget->setVisible(settings()->value("acquisition/UsAcqShowDetails").toBool());

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mLayout->addWidget(mOptionsWidget);

	mLayout->addStretch();
	mLayout->addWidget(mTimedAlgorithmProgressBar);
}

USAcqusitionWidget::~USAcqusitionWidget()
{
}

QString USAcqusitionWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>US Acquisition.</h3>"
		"<p><i>Record and reconstruct US data.</i></br>"
		"</html>";
}


void USAcqusitionWidget::toggleDetailsSlot()
{
  mOptionsWidget->setVisible(!mOptionsWidget->isVisible());
  settings()->setValue("acquisition/UsAcqShowDetails", mOptionsWidget->isVisible());
}

QWidget* USAcqusitionWidget::createOptionsWidget()
{
	QWidget* retval = new QWidget(this);
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);

	SoundSpeedConverterWidget* soundSpeedWidget = new SoundSpeedConverterWidget(this);
	connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), soundSpeedWidget, SLOT(setToolSlot(const QString&)));

	ProbeConfigWidget* probeWidget = new ProbeConfigWidget(this);
	probeWidget->getActiveProbeConfigWidget()->setVisible(false);

	SpinBoxGroupWidget* temporalCalibrationWidget = new SpinBoxGroupWidget(this, DoubleDataAdapterTimeCalibration::New());

	int line = 0;
	layout->addWidget(this->createHorizontalLine(), line++, 0, 1, 1);
	layout->addWidget(this->wrapGroupBox(probeWidget, "Probe", "Probe Definition"), line++, 0);
	layout->addWidget(this->wrapGroupBox(soundSpeedWidget, "Sound Speed", "Sound Speed"), line++, 0);
	layout->addWidget(temporalCalibrationWidget, line++, 0);

	return retval;
}

QWidget* USAcqusitionWidget::wrapGroupBox(QWidget* input, QString name, QString tip)
{
	QGroupBox* retval = new QGroupBox(name);
	retval->setToolTip(tip);
	QVBoxLayout* layout = new QVBoxLayout(retval);
	layout->addWidget(input);
	layout->addStretch();
	layout->setMargin(layout->margin()/2);
	return retval;
}

QWidget* USAcqusitionWidget::wrapVerticalStretch(QWidget* input)
{
	QWidget* retval = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(retval);
	layout->addWidget(input);
	layout->addStretch();
	layout->setMargin(0);
	layout->setSpacing(0);
	return retval;
}

void USAcqusitionWidget::acquisitionDataReadySlot()
{
	if (settings()->value("Automation/autoReconstruct").toBool())
	{
		mPluginData->getReconstructer()->startReconstruction();
	}
}

void USAcqusitionWidget::acquisitionStateChangedSlot()
{
	Acquisition::STATE state = mBase->getState();

	switch (state)
	{
	case Acquisition::sRUNNING :
		break;
	case Acquisition::sNOT_RUNNING :
		break;
	case Acquisition::sPOST_PROCESSING :
		break;
	}
}

void USAcqusitionWidget::recordStarted()
{
	mDisplayTimerWidget->start();
}
void USAcqusitionWidget::recordStopped()
{
	mDisplayTimerWidget->stop();
}
void USAcqusitionWidget::recordCancelled()
{
	mDisplayTimerWidget->stop();
}

void USAcqusitionWidget::reconstructAboutToStartSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mPluginData->getReconstructer()->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->attach(threads);
}

void USAcqusitionWidget::reconstructStartedSlot()
{
	mBase->startPostProcessing();
}

void USAcqusitionWidget::reconstructFinishedSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mPluginData->getReconstructer()->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->detach(threads);
	mBase->stopPostProcessing();
}

}//namespace cx
