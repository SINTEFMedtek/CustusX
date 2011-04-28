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
  WhatsThisWidget(parent, "ActiveVolumeWidget", "Active Volume")
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);

  ssc::LabeledComboBoxWidget*  combo = new ssc::LabeledComboBoxWidget(this, ActiveImageStringDataAdapter::New());
  layout->addWidget(combo);
}

QString ActiveVolumeWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Active volume</h3>"
      "<p>Displays the currently selected active volume.</p>"
      "<p><i>Use the list to change the active volume.</i></p>"
      "</html>";
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

VolumeInfoWidget::VolumeInfoWidget(QWidget* parent) :
  WhatsThisWidget(parent, "VolumeInfoWidget", "Volume Info")
{
  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

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

QString VolumeInfoWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Volume information</h3>"
      "<p>Displays information about a selected volume.</p>"
      "<p><i></i></p>"
      "</html>";
}

void VolumeInfoWidget::deleteDataSlot()
{
  if (!ssc::dataManager()->getActiveImage())
    return;
  ssc::dataManager()->removeData(ssc::dataManager()->getActiveImage()->getUid());
}


void VolumeInfoWidget::updateSlot()
{
  mParentFrameAdapter->setData(ssc::dataManager()->getActiveImage());
  mNameAdapter->setData(ssc::dataManager()->getActiveImage());
  mUidAdapter->setData(ssc::dataManager()->getActiveImage());
}
/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

VolumePropertiesWidget::VolumePropertiesWidget(QWidget* parent) :
    WhatsThisWidget(parent, "VolumePropertiesWidget", "Volume Properties")
{
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

QString VolumePropertiesWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Volume properties</h3>"
      "<p>Displays and adjusts information about a selected volume.</p>"
      "<p><i>Click the tabs to see what properties can be changed.</i></p>"
      "</html>";
}

}
