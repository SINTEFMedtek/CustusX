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
#include "sscReconstructer.h"
#include "cxProbeConfigWidget.h"

namespace cx
{


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

	this->createAction(this,
	      QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png"),
	      "Details", "Show Advanced Settings",
	      SLOT(toggleDetailsSlot()),
	      mLayout);

	mOptionsWidget = this->createOptionsWidget();
	mOptionsWidget->setVisible(settings()->value("acquisition/UsAcqShowDetails").toBool());

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mLayout->addWidget(mOptionsWidget);

	mLayout->addStretch();
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

	//for testing sound speed converting - BEGIN
	SoundSpeedConverterWidget* soundSpeedWidget = new SoundSpeedConverterWidget(this);
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), soundSpeedWidget,
		SLOT(setToolSlot(const QString&)));
	//for testing sound speed converting - END

	int line = 0;

	layout->addWidget(this->createHorizontalLine(), line, 0, 1, 1);
	++line;

	// define cropping group
	QGroupBox* probeGroupBox = new QGroupBox("Probe");
	probeGroupBox->setToolTip("Probe Definition");
	QVBoxLayout* probeLayout = new QVBoxLayout(probeGroupBox);
	ProbeConfigWidget* probeWidget = new ProbeConfigWidget(this);
	probeWidget->getActiveProbeConfigWidget()->setVisible(false);
	probeLayout->addWidget(probeWidget);
	probeLayout->setMargin(probeLayout->margin()/2);
//	layout->addWidget(probeWidget, line, 0);
	layout->addWidget(probeGroupBox, line, 0);
	++line;
	layout->addWidget(soundSpeedWidget, line, 0);
	++line;
	layout->addWidget(new ssc::SpinBoxGroupWidget(this, DoubleDataAdapterTimeCalibration::New()), line, 0);
	++line;


	return retval;
}

void USAcqusitionWidget::postProcessingSlot(QString sessionId)
{
	mAcquisition->saveSession(sessionId, mPluginData->getReconstructer()->getParams()->mAngioAdapter->getValue());
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
