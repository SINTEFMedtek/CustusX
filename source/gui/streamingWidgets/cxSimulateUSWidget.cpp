#include "cxSimulateUSWidget.h"

#include <QVBoxLayout>
#include "sscLabeledComboBoxWidget.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxVideoService.h"
#include "cxVideoConnectionManager.h"
#include "cxVideoConnection.h"
#include "cxDirectlyLinkedImageReceiverThread.h"

namespace cx
{
SimulateUSWidget::SimulateUSWidget(QWidget* parent) :
		BaseWidget(parent, "SimulateUSWidget", "Simulated US"),
		mImageSelector(SelectImageStringDataAdapter::New())
{
	ssc::LabeledComboBoxWidget* imageCombo = new ssc::LabeledComboBoxWidget(this, mImageSelector);
	connect(mImageSelector.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	this->imageChangedSlot(mImageSelector->getValue());

	mTopLayout = new QVBoxLayout(this);
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
}

} /* namespace cx */
