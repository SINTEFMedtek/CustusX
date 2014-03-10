#include "cxVolumeInfoWidget.h"

#include <QPushButton>
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscLabeledLineEditWidget.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscVolumeHelpers.h"
#include "cxActiveImageProxy.h"

namespace cx
{

VolumeInfoWidget::VolumeInfoWidget(QWidget* parent) :
  InfoWidget(parent, "VolumeInfoWidget", "Volume Info")
{
	mActiveImageProxy = ActiveImageProxy::New(dataService());
	connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(updateSlot()));

	this->addWidgets();

	this->updateSlot();
}

VolumeInfoWidget::~VolumeInfoWidget()
{}

void VolumeInfoWidget::addWidgets()
{
	mParentFrameAdapter = ParentFrameStringDataAdapter::New();
	mNameAdapter = DataNameEditableStringDataAdapter::New();
	mUidAdapter = DataUidEditableStringDataAdapter::New();
	mModalityAdapter = DataModalityStringDataAdapter::New();
	mImageTypeAdapter = ImageTypeStringDataAdapter::New();

	int i=0;
	new LabeledLineEditWidget(this, mUidAdapter, gridLayout, i++);
	new LabeledLineEditWidget(this, mNameAdapter, gridLayout, i++);
	new LabeledComboBoxWidget(this, mModalityAdapter, gridLayout, i++);
	new LabeledComboBoxWidget(this, mImageTypeAdapter, gridLayout, i++);
	new LabeledComboBoxWidget(this, mParentFrameAdapter, gridLayout, i++);

	gridLayout->addWidget(mTableWidget, i++, 0, 1, 2);
}

QString VolumeInfoWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Volume information</h3>"
      "<p>Displays information about a selected volume.</p>"
      "<p><i></i></p>"
      "</html>";
}

void VolumeInfoWidget::updateSlot()
{
	ImagePtr image = dataManager()->getActiveImage();
	mParentFrameAdapter->setData(image);
	mNameAdapter->setData(image);
	mUidAdapter->setData(image);
	mModalityAdapter->setData(image);
	mImageTypeAdapter->setData(image);

	std::map<std::string, std::string> info = getDisplayFriendlyInfo(image);
	this->populateTableWidget(info);
}
}//namespace
