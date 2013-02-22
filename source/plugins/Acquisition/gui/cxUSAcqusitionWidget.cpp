#include "cxUSAcqusitionWidget.h"

#include <QtGui>
#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscDoubleWidgets.h"
#include "sscTypeConversions.h"
#include "cxPatientData.h"
#include "cxSoundSpeedConversionWidget.h"
#include "cxRecordSessionWidget.h"
#include "cxSettings.h"
#include "cxToolDataAdapters.h"
#include "cxDoubleDataAdapterTemporalCalibration.h"
#include "sscReconstructManager.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxProbeConfigWidget.h"
#include "cxDisplayTimerWidget.h"


namespace cx
{

USAcqusitionWidget::USAcqusitionWidget(AcquisitionDataPtr pluginData, QWidget* parent) :
	RecordBaseWidget(pluginData, parent, settings()->value("Ultrasound/acquisitionName").toString())
{
	this->setObjectName("USAcqusitionWidget");
	this->setWindowTitle("US Acquisition");

	// connect to reconstructer signals
	connect(mPluginData->getReconstructer().get(), SIGNAL(reconstructAboutToStart()), this, SLOT(reconstructAboutToStartSlot()));
	mAcquisition.reset(new USAcquisition(mBase));
	connect(mAcquisition.get(), SIGNAL(acquisitionDataReady()), this, SLOT(acquisitionDataReadySlot()));

	connect(mBase.get(), SIGNAL(stateChanged()), this, SLOT(acquisitionStateChangedSlot()));
	connect(mBase.get(), SIGNAL(started()), this, SLOT(recordStarted()));
	connect(mBase.get(), SIGNAL(stopped()), this, SLOT(recordStopped()));
	connect(mBase.get(), SIGNAL(cancelled()), this, SLOT(recordCancelled()));

	mRecordSessionWidget->setDescriptionVisibility(false);

	QHBoxLayout* timerLayout = new QHBoxLayout;
	mLayout->addLayout(timerLayout);
	mDisplayTimerWidget = new DisplayTimerWidget(this);
	mDisplayTimerWidget ->setFontSize(10);
	timerLayout->addStretch();
	timerLayout->addWidget(mDisplayTimerWidget);
	timerLayout->addStretch();

	QGridLayout* editsLayout = new QGridLayout;
//	editsLayout->setMargin(0);
	editsLayout->setColumnStretch(0,0);
	editsLayout->setColumnStretch(1,1);
	RecordBaseWidget::mLayout->addLayout(editsLayout);
	new ssc::LabeledComboBoxWidget(this, ActiveProbeConfigurationStringDataAdapter::New(), editsLayout, 0);
	new ssc::LabeledComboBoxWidget(this, mPluginData->getReconstructer()->getParams()->mPresetTFAdapter, editsLayout, 1);

	QAction* optionsAction = this->createAction(this,
	      QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png"),
	      "Details", "Show Advanced Settings",
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

void USAcqusitionWidget::reconstructAboutToStartSlot()
{
	std::set<cx::TimedAlgorithmPtr> reconstructer = mPluginData->getReconstructer()->getThreadedReconstruction();
	std::set<cx::TimedAlgorithmPtr>::iterator iter;
	for(iter=reconstructer.begin(); iter!=reconstructer.end(); ++iter)
	{
		connect((*iter).get(), SIGNAL(started(int)), this, SLOT(reconstructStartedSlot()));
		connect((*iter).get(), SIGNAL(finished()), this, SLOT(reconstructFinishedSlot()));

		mTimedAlgorithmProgressBar->attach(*iter);
	}
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
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), soundSpeedWidget, SLOT(setToolSlot(const QString&)));

	ProbeConfigWidget* probeWidget = new ProbeConfigWidget(this);
	probeWidget->getActiveProbeConfigWidget()->setVisible(false);

	ssc::SpinBoxGroupWidget* temporalCalibrationWidget = new ssc::SpinBoxGroupWidget(this, DoubleDataAdapterTimeCalibration::New());

	int line = 0;
	layout->addWidget(this->createHorizontalLine(), line++, 0, 1, 1);
	layout->addWidget(this->wrapGroupBox(probeWidget, "Probe", "Probe Definition"), line++, 0);
	layout->addWidget(this->wrapGroupBox(soundSpeedWidget, "Sound Speed", "Sound Speed"), line++, 0);
	layout->addWidget(temporalCalibrationWidget, line++, 0);

// alternative: group advanced widgets as tabs:
//	QTabWidget* tabWidget = new QTabWidget(this);
//	layout->addWidget(tabWidget, 0, 0);
//	tabWidget->addTab(this->addVerticalStretch(probeWidget), "Probe");
//	tabWidget->addTab(this->addVerticalStretch(soundSpeedWidget), "Sound Speed");
//	tabWidget->addTab(this->addVerticalStretch(temporalCalibrationWidget), "Temporal Calibration");

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



void USAcqusitionWidget::reconstructStartedSlot()
{
	// TODO reconstruction is still controlled from the widget. Move this to USAcquisition, but keep in mind
	// that temp cal also uses that class. - configurable

	mBase->startPostProcessing();
}

void USAcqusitionWidget::reconstructFinishedSlot()
{
	// stop if all threads are finished
	bool finished = true;
	std::set<cx::TimedAlgorithmPtr> reconstructer = mPluginData->getReconstructer()->getThreadedReconstruction();
	std::set<cx::TimedAlgorithmPtr>::iterator iter;
	for(iter=reconstructer.begin(); iter!=reconstructer.end(); ++iter)
	{
		finished = finished && (*iter)->isFinished();
		if ((*iter)->isFinished())
			mTimedAlgorithmProgressBar->detach(*iter);
	}

	if (finished)
		mBase->stopPostProcessing();
}

}//namespace cx
