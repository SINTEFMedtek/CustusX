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

	mTopLayout = new QVBoxLayout(this);
	mTopLayout->addWidget(imageCombo);
}

SimulateUSWidget::~SimulateUSWidget()
{}

QString SimulateUSWidget::defaultWhatsThis() const
{
	return "TODO";
}

void SimulateUSWidget::imageChangedSlot(QString imageUid)
{
	ImageReceiverThreadPtr client = videoService()->getVideoConnection()->getVideoConnection()->getClient();
	DirectlyLinkedImageReceiverThreadPtr casted_client = boost::dynamic_pointer_cast<DirectlyLinkedImageReceiverThread>(client);
	if(casted_client)
		casted_client->setImageToStream(imageUid);

}

} /* namespace cx */
