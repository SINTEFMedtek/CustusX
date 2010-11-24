#include "cxVolumePropertiesWidget.h"
#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "sscLabeledComboBoxWidget.h"
#include "sscLabeledLineEditWidget.h"
#include "sscImage.h"
#include "cxTransferFunctionWidget.h"
#include "cxCroppingWidget.h"
#include "cxClippingWidget.h"
#include "cxShadingWidget.h"
#include "cxDataInterface.h"
#include "sscDataManager.h"

namespace cx
{

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

ActiveVolumeWidget::ActiveVolumeWidget(QWidget* parent) :
  QWidget(parent)
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setObjectName("ActiveVolumeWidget");
  layout->setMargin(0);

  ssc::LabeledComboBoxWidget*  combo = new ssc::LabeledComboBoxWidget(this, ActiveImageStringDataAdapter::New());
  layout->addWidget(combo);
}


/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------


VolumeInfoWidget::VolumeInfoWidget(QWidget* parent) :
  QWidget(parent)
{
  this->setObjectName("VolumeInfoWidget");
  this->setWindowTitle("Info");

  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);


  QGridLayout* gridLayout = new QGridLayout;
  toptopLayout->addLayout(gridLayout);

  QPushButton* deleteButton = new QPushButton("Delete", this);
  deleteButton->setToolTip("Remove the selected Image from the system.");
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteDataSlot()));


  mParentFrameAdapter = ParentFrameStringDataAdapter::New();
  ssc::LabeledComboBoxWidget*  parentFrame = new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter);

  mNameAdapter = DataNameEditableStringDataAdapter::New();
  ssc::LabeledLineEditWidget*  nameEdit = new ssc::LabeledLineEditWidget(this, mNameAdapter);

  mUidAdapter = DataUidEditableStringDataAdapter::New();
  ssc::LabeledLineEditWidget*  uidEdit = new ssc::LabeledLineEditWidget(this, mUidAdapter);

  gridLayout->addWidget(uidEdit, 0, 0);
  gridLayout->addWidget(nameEdit, 1, 0);
  gridLayout->addWidget(parentFrame, 2, 0);
  gridLayout->addWidget(deleteButton, 3, 0);

  toptopLayout->addStretch();

  connect(ssc::dataManager(), SIGNAL(activeImageChanged(const QString&)), this, SLOT(updateSlot()));
  updateSlot();
}

VolumeInfoWidget::~VolumeInfoWidget()
{
}

void VolumeInfoWidget::deleteDataSlot()
{
  if (ssc::dataManager()->getActiveImage())
    return;
  ssc::dataManager()->removeData(ssc::dataManager()->getActiveImage()->getUid());
}


void VolumeInfoWidget::updateSlot()
{
  mParentFrameAdapter->setData(ssc::dataManager()->getActiveImage());
  mNameAdapter->setData(ssc::dataManager()->getActiveImage());
  mUidAdapter->setData(ssc::dataManager()->getActiveImage());
}

void VolumeInfoWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void VolumeInfoWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------


VolumePropertiesWidget::VolumePropertiesWidget(QWidget* parent) : QWidget(parent)
{
  this->setObjectName("VolumePropertiesWidget");
  this->setWindowTitle("Volume Properties");

  QVBoxLayout* layout = new QVBoxLayout(this);

  layout->addWidget(new ActiveVolumeWidget(this));

  QTabWidget* tabWidget = new QTabWidget(this);
  layout->addWidget(tabWidget);
  tabWidget->addTab(new VolumeInfoWidget(this), "Info");
  tabWidget->addTab(new TransferFunctionWidget(this), QString("Transfer Functions"));
  tabWidget->addTab(new ShadingWidget(this), "Shading");
  tabWidget->addTab(new CroppingWidget(this), "Crop");
  tabWidget->addTab(new ClippingWidget(this), "Clip");
}

}
