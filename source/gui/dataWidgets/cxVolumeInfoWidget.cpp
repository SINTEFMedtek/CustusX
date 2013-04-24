#include "cxVolumeInfoWidget.h"

#include <QPushButton>
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscLabeledLineEditWidget.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxActiveImageProxy.h"

namespace cx
{

VolumeInfoWidget::VolumeInfoWidget(QWidget* parent) :
  BaseWidget(parent, "VolumeInfoWidget", "Volume Info")
{
  QVBoxLayout* topLayout = new QVBoxLayout(this);
  QGridLayout* gridLayout = new QGridLayout;
  topLayout->addLayout(gridLayout);

  /*
  QPushButton* deleteButton = new QPushButton("Delete", this);
  deleteButton->setToolTip("Remove the selected Image from the system.");
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteDataSlot()));
  */

  mParentFrameAdapter = ParentFrameStringDataAdapter::New();
  mNameAdapter = DataNameEditableStringDataAdapter::New();
  mUidAdapter = DataUidEditableStringDataAdapter::New();
  mModalityAdapter = DataModalityStringDataAdapter::New();
  mImageTypeAdapter = ImageTypeStringDataAdapter::New();

  int i=0;
  gridLayout->addWidget(new ssc::LabeledLineEditWidget(this, mUidAdapter), i++, 0, 1, 2);
  new ssc::LabeledLineEditWidget(this, mNameAdapter, gridLayout, i++);
  new ssc::LabeledComboBoxWidget(this, mModalityAdapter, gridLayout, i++);
  new ssc::LabeledComboBoxWidget(this, mImageTypeAdapter, gridLayout, i++);
  new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter, gridLayout, i++);

  //gridLayout->addWidget(deleteButton, i++, 0, 1, 2);

  topLayout->addStretch();

  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(updateSlot()));

  this->updateSlot();
}

VolumeInfoWidget::~VolumeInfoWidget()
{}

QString VolumeInfoWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Volume information</h3>"
      "<p>Displays information about a selected volume.</p>"
      "<p><i></i></p>"
      "</html>";
}

/*
void VolumeInfoWidget::deleteDataSlot()
{
  if (!ssc::dataManager()->getActiveImage())
    return;
  ssc::dataManager()->removeData(ssc::dataManager()->getActiveImage()->getUid());
}
*/

void VolumeInfoWidget::updateSlot()
{
	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	mParentFrameAdapter->setData(image);
	mNameAdapter->setData(image);
	mUidAdapter->setData(image);
	mModalityAdapter->setData(image);
	mImageTypeAdapter->setData(image);
}
}//namespace
