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

namespace cx
{

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DisplayTimerWidget::DisplayTimerWidget(QWidget* parent) : QWidget(parent)
{
	mTimer = new QTimer;
	connect(mTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
	mTimer->setInterval(1000);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(0);

	mLabel = new QLabel;
	layout->addWidget(mLabel);
}

void DisplayTimerWidget::start()
{
	mStartTime = QDateTime::currentDateTime();
	mTimer->start();
	timeoutSlot();
}

void DisplayTimerWidget::stop()
{
	mTimer->stop();
	int secs = mStartTime.secsTo(QDateTime::currentDateTime());
	mLabel->setText(QString("<font size=10 color=green><b>%1 s</b></font>").arg(secs));
}

void DisplayTimerWidget::timeoutSlot()
{
	int secs = mStartTime.secsTo(QDateTime::currentDateTime());
	mLabel->setText(QString("<font size=10 color=black><b>%1 s</b></font>").arg(secs));
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


USAcqusitionWidget::USAcqusitionWidget(AcquisitionDataPtr pluginData, QWidget* parent) :
	RecordBaseWidget(pluginData, parent, settings()->value("Ultrasound/acquisitionName").toString())
{
	this->setObjectName("USAcqusitionWidget");
	this->setWindowTitle("US Acquisition");

	mAcquisition.reset(new USAcquisition(pluginData));
	connect(mAcquisition.get(), SIGNAL(ready(bool,QString)), mRecordSessionWidget, SLOT(setReady(bool,QString)));
	//  connect(mAcquisition.get(), SIGNAL(ready(bool)), this, SIGNAL(ready(bool)));
	connect(mAcquisition.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(saveDataCompletedSlot(QString)));

	mRecordSessionWidget->setDescriptionVisibility(false);

	QHBoxLayout* timerLayout = new QHBoxLayout;
	mLayout->addLayout(timerLayout);
	mDisplayTimerWidget = new DisplayTimerWidget(this);
	timerLayout->addStretch();
	timerLayout->addWidget(mDisplayTimerWidget);
	timerLayout->addStretch();

	//for testing sound speed converting - BEGIN
	SoundSpeedConverterWidget* soundSpeedWidget = new SoundSpeedConverterWidget(this);
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), soundSpeedWidget,
		SLOT(setToolSlot(const QString&)));
	//for testing sound speed converting - END

	RecordBaseWidget::mLayout->addWidget(new ssc::LabeledComboBoxWidget(this,
		ActiveToolConfigurationStringDataAdapter::New()));
	mLayout->addStretch();
	mLayout->addWidget(soundSpeedWidget);
	RecordBaseWidget::mLayout->addWidget(new ssc::SpinBoxGroupWidget(this, DoubleDataAdapterTimeCalibration::New()));

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mLayout->addWidget(mTimedAlgorithmProgressBar);

	mAcquisition->checkIfReadySlot();
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

void USAcqusitionWidget::postProcessingSlot(QString sessionId)
{
	mAcquisition->saveSession(sessionId);
}

void USAcqusitionWidget::saveDataCompletedSlot(QString mhdFilename)
{
	mPluginData->getReconstructer()->selectData(mhdFilename);

	if (settings()->value("Automation/autoReconstruct").toBool())
	{
		mThreadedTimedReconstructer.reset(new ssc::ThreadedTimedReconstructer(mPluginData->getReconstructer()));
		//    mThreadedReconstructer.reset(new ssc::ThreadedReconstructer(mPluginData->getReconstructer()));
		mTimedAlgorithmProgressBar->attach(mThreadedTimedReconstructer);
		connect(mThreadedTimedReconstructer.get(), SIGNAL(finished()), this, SLOT(reconstructFinishedSlot()));
		mThreadedTimedReconstructer->start();
		mRecordSessionWidget->startPostProcessing("Reconstructing");
	}
}

void USAcqusitionWidget::reconstructFinishedSlot()
{
	mRecordSessionWidget->stopPostProcessing();
	mTimedAlgorithmProgressBar->detach(mThreadedTimedReconstructer);
//	mThreadedReconstructer.reset();
}

void USAcqusitionWidget::startedSlot()
{
	mAcquisition->startRecord();
	mRecordSessionWidget->setDescription(settings()->value("Ultrasound/acquisitionName").toString());
	mDisplayTimerWidget->start();
}

void USAcqusitionWidget::stoppedSlot()
{
//	if (mThreadedReconstructer)
//	{
//		// TODO Did not work - crashes
//		mThreadedReconstructer->terminate();
//		mThreadedReconstructer->wait();
//		mPluginData->getReconstructer()->selectData(mPluginData->getReconstructer()->getSelectedData());
//		// TODO perform cleanup of all resources connected to this recording.
//	}

	mDisplayTimerWidget->stop();
	mAcquisition->stopRecord();
}
}//namespace cx
