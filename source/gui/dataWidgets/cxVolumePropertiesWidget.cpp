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
#include "cxDataSelectWidget.h"

namespace cx
{

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

ActiveVolumeWidget::ActiveVolumeWidget(QWidget* parent) :
  BaseWidget(parent, "ActiveVolumeWidget", "Active Volume")
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
//  ssc::LabeledComboBoxWidget*  combo = new ssc::LabeledComboBoxWidget(this, ActiveImageStringDataAdapter::New());
  layout->addWidget(new DataSelectWidget(this, ActiveImageStringDataAdapter::New()));
//  layout->addWidget(combo);
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
  BaseWidget(parent, "VolumeInfoWidget", "Volume Info")
{
  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

  QGridLayout* gridLayout = new QGridLayout;
  toptopLayout->addLayout(gridLayout);

  QPushButton* deleteButton = new QPushButton("Delete", this);
  deleteButton->setToolTip("Remove the selected Image from the system.");
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteDataSlot()));

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

//  int i=0;
//  gridLayout->addWidget(uidEdit,        i++, 0);
//  gridLayout->addWidget(nameEdit,       i++, 0);
//  gridLayout->addWidget(modalityCombo,  i++, 0);
//  gridLayout->addWidget(imageTypeCombo, i++, 0);
//  gridLayout->addWidget(parentFrame,    i++, 0);
  gridLayout->addWidget(deleteButton, i++, 0, 1, 2);

  toptopLayout->addStretch();

  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(updateSlot()));
  //TODO: Check if the following are needed
//  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateSlot()));
//  connect(mActiveImageProxy.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(updateSlot()));
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
	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	mParentFrameAdapter->setData(image);
	mNameAdapter->setData(image);
	mUidAdapter->setData(image);
	mModalityAdapter->setData(image);
	mImageTypeAdapter->setData(image);
}
/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

VolumePropertiesWidget::VolumePropertiesWidget(QWidget* parent) :
    BaseWidget(parent, "VolumePropertiesWidget", "Volume Properties")
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
