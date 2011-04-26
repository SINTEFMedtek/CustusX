#include <cxConnectedThresholdImageFilterWidget.h>

#include "cxDataManager.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{

ConnectedThresholdImageFilterWidget::ConnectedThresholdImageFilterWidget(QWidget* parent) :
    WhatsThisWidget(parent, "ConnectedThresholdImageFilterWidget", "Connected Threshold Image Filter"),
    mStatusLabel(new QLabel(""))
{
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  connect(&mConnectedThresholdImageFilter, SIGNAL(finished()), this, SLOT(handleFinishedSlot()));

  QPushButton* segmentButton = new QPushButton("Segment", this);
  connect(segmentButton, SIGNAL(clicked()), this, SLOT(segmentSlot()));

  QPushButton* segmentationOptionsButton = new QPushButton("Options", this);
  segmentationOptionsButton->setCheckable(true);

  QGroupBox* segmentationOptionsWidget = this->createGroupbox(this->createSegmentationOptionsWidget(), "Segmentation options");
  connect(segmentationOptionsButton, SIGNAL(clicked(bool)), segmentationOptionsWidget, SLOT(setVisible(bool)));
  segmentationOptionsWidget->setVisible(segmentationOptionsButton->isChecked());

  topLayout->addWidget(segmentButton, 1,0);
  topLayout->addWidget(segmentationOptionsButton, 1,1);
  topLayout->addWidget(segmentationOptionsWidget, 2, 0, 1, 2);
  topLayout->addWidget(mStatusLabel);
}

ConnectedThresholdImageFilterWidget::~ConnectedThresholdImageFilterWidget()
{
}

QString ConnectedThresholdImageFilterWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Connected Threshold Image Filter.</h3>"
      "<p>Labels pixels with a ReplaceValue that are connected to an initial seed and lies within a lower and upper threshold range.</p>"
      "<p><i></i></p>"
      "</html>";
}

void ConnectedThresholdImageFilterWidget::handleFinishedSlot()
{
  ssc::ImagePtr segmentedImage = mConnectedThresholdImageFilter.getOutput();
  if(!segmentedImage)
    return;

  mStatusLabel->setText("<font color=green> Done. </font>\n");

//  emit outputImageChanged(segmentedImage->getUid());
}

void ConnectedThresholdImageFilterWidget::segmentSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
//  this->revertTransferFunctions();

  //TODO hack
  ssc::ImagePtr selectedImage = ssc::dataManager()->getActiveImage();
  float lowerThreshold = 300.0;
  float upperThreshold = 1000.0;
  int replaceValue = 1;
  itkImageType::IndexType seed;
  seed[0] = 100;
  seed[1] = 100;
  seed[2] = 100;
  //hack

  mConnectedThresholdImageFilter.setInput(selectedImage, outputBasePath, lowerThreshold, upperThreshold, replaceValue, seed);

  mStatusLabel->setText("<font color=orange> Generating segmentation... Please wait!</font>\n");
}

QWidget* ConnectedThresholdImageFilterWidget::createSegmentationOptionsWidget()
{
  QWidget* retval = new QWidget();
  return retval;
}
}
