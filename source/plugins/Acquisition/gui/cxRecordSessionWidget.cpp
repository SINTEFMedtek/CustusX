#include "cxRecordSessionWidget.h"

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QEvent>
#include "sscTime.h"
#include "sscMessageManager.h"
#include "cxRecordSession.h"
#include "cxToolManager.h"
#include "sscTypeConversions.h"

namespace cx
{
RecordSessionWidget::RecordSessionWidget(AcquisitionPtr base, QWidget* parent, QString defaultDescription) :
    BaseWidget(parent, "RecordSessionWidget", "Record Session"),
    mBase(base),
    mInfoLabel(new QLabel("")),
    mStartStopButton(new QPushButton(QIcon(":/icons/open_icon_library/png/64x64/actions/media-record-3.png"), "Start")),
    mCancelButton(new QPushButton(QIcon(":/icons/open_icon_library/png/64x64/actions/process-stop-7.png"), "Cancel")),
    mDescriptionLine(new QLineEdit(defaultDescription))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  mDescriptionLabel = new QLabel("Description:");
  layout->addWidget(mInfoLabel);
  layout->addWidget(mDescriptionLabel);
  layout->addWidget(mDescriptionLine);
  layout->addWidget(mStartStopButton);
  layout->addWidget(mCancelButton);

  connect(mBase.get(), SIGNAL(stateChanged()), this, SLOT(recordStateChangedSlot()));
  connect(mBase.get(), SIGNAL(readinessChanged()), this, SLOT(readinessChangedSlot()));

  mStartStopButton->setCheckable(true);
  connect(mStartStopButton, SIGNAL(clicked(bool)), this, SLOT(startStopSlot(bool)));
  connect(mCancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelSlot()));

  this->recordStateChangedSlot();
  this->readinessChangedSlot();
}

QString RecordSessionWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Record session.</h3>"
    "<p>Lets you record a session of some kind.</p>"
    "<p><i></i></p>"
    "</html>";
}

void RecordSessionWidget::setReady(bool val, QString text)
{
  this->setEnabled(val);
  mInfoLabel->setText(text);
}

void RecordSessionWidget::setDescriptionVisibility(bool value)
{
    mDescriptionLine->setVisible(value);
    mDescriptionLabel->setVisible(value);
}

RecordSessionWidget::~RecordSessionWidget()
{}

void RecordSessionWidget::setDescription(QString text)
{
  mDescriptionLine->setText(text);
}

void RecordSessionWidget::readinessChangedSlot()
{
	this->setEnabled(mBase->isReady());
    mInfoLabel->setText(mBase->getInfoText());
}

void RecordSessionWidget::recordStateChangedSlot()
{
	Acquisition::STATE state = mBase->getState();

	mStartStopButton->blockSignals(true);

	switch (state)
	{
	case Acquisition::sRUNNING :
	    mStartStopButton->setChecked(true);
		mStartStopButton->setText("Stop");
		mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-stop.png"));
	    mStartStopButton->setEnabled(true);
	    mCancelButton->setEnabled(true);
		break;
	case Acquisition::sNOT_RUNNING :
	    mStartStopButton->setChecked(false);
		mStartStopButton->setText("Start");
		mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/png/64x64/actions/media-record-3.png"));
	    mStartStopButton->setEnabled(true);
		mCancelButton->setEnabled(false);
		break;
	case Acquisition::sPOST_PROCESSING :
	    mStartStopButton->setChecked(false);
		mStartStopButton->setText("Processing...");
	    mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/png/64x64/actions/media-record-3.png"));
	    mStartStopButton->setEnabled(false);
	    mCancelButton->setEnabled(false);
		break;
	}

	mStartStopButton->blockSignals(false);
}

void RecordSessionWidget::startStopSlot(bool checked)
{
	mBase->toggleRecord();
}

void RecordSessionWidget::cancelSlot()
{
	mBase->cancelRecord();
}

}
