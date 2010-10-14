#include "cxSegmentationWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QGridLayout>
#include <QCheckBox>

#include "sscDataManager.h"
#include "sscMessageManager.h"
#include "sscDoubleWidgets.h"
#include "cxDataInterface.h"
#include "cxVolumePropertiesWidget.h"
#include "cxSegmentation.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{

SegmentationWidget::SegmentationWidget(QWidget* parent) :
  WhatsThisWidget(parent), mThreshold(100), mBinary(false), mUseSmothing(true), mSmoothSigma(0.5)
{
  this->setObjectName("SegmentationWidget");
  this->setWindowTitle("Segmentation");

  QGridLayout* toptopLayout = new QGridLayout(this);

  toptopLayout->addWidget(new ActiveVolumeWidget(this), 0, 0);

  QPushButton* segmentButton = new QPushButton("Segment", this);
  connect(segmentButton, SIGNAL(clicked()), this, SLOT(segmentSlot()));
  QPushButton* segmentationOptionsButton = new QPushButton("Options", this);
  segmentationOptionsButton->setCheckable(true);
  toptopLayout->addWidget(segmentButton, 1,0);
  toptopLayout->addWidget(segmentationOptionsButton, 1,1);
  QGroupBox* segmentationOptionsGroupBox = this->createGroupBox(this->createSegmentationOptionsWidget(), "Segmentation options");
  connect(segmentationOptionsButton, SIGNAL(clicked(bool)), segmentationOptionsGroupBox, SLOT(setVisible(bool)));
  connect(segmentationOptionsButton, SIGNAL(clicked()), this, SLOT(adjustSizeSlot()));
  segmentationOptionsGroupBox->setVisible(segmentationOptionsButton->isChecked());
  toptopLayout->addWidget(segmentationOptionsGroupBox, 2, 0, 1, 2);

  QPushButton* contourButton = new QPushButton("Contour", this);
  connect(contourButton, SIGNAL(clicked()), this, SLOT(contourSlot()));
  QPushButton* contourOptionsButton = new QPushButton("Options", this);
  contourOptionsButton->setCheckable(true);
  toptopLayout->addWidget(contourButton, 3,0);
  toptopLayout->addWidget(contourOptionsButton,3,1);
  QGroupBox* contourOptionsGroupBox = this->createGroupBox(this->createCotourOptionsWidget(), "Contour options");
  connect(contourOptionsButton, SIGNAL(clicked(bool)), contourOptionsGroupBox, SLOT(setVisible(bool)));
  connect(contourOptionsButton, SIGNAL(clicked()), this, SLOT(adjustSizeSlot()));
  contourOptionsGroupBox->setVisible(contourOptionsButton->isChecked());
  toptopLayout->addWidget(contourOptionsGroupBox, 4, 0, 1, 2);

  this->resize(this->sizeHint());
}

SegmentationWidget::~SegmentationWidget()
{
}

QString SegmentationWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Segmentation.</h3>"
    "<p><i>Segment out areas from the selected image using a threshold.</i></p>"
    "</html>";
}

void SegmentationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void SegmentationWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


void SegmentationWidget::segmentSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();

  Segmentation().segment(ssc::dataManager()->getActiveImage(), outputBasePath, mThreshold, mUseSmothing, mSmoothSigma);
}

void SegmentationWidget::contourSlot()
{
  int threshold = 100;

  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();

  Segmentation().contour(ssc::dataManager()->getActiveImage(), outputBasePath, threshold);
}

void SegmentationWidget::adjustSizeSlot()
{
  this->adjustSize();
  this->parentWidget()->adjustSize();
}

QWidget* SegmentationWidget::createSegmentationOptionsWidget()
{
  QWidget* retval = new QWidget(this);
  QGridLayout* layout = new QGridLayout(retval);

  QSpinBox* thresholdSpinBox = new QSpinBox();
  QLabel* thresholdLabel = new QLabel("Threshold");

  QCheckBox* binaryCheckbox = new QCheckBox();
  binaryCheckbox->setChecked(false);
  QLabel* binaryLabel = new QLabel("Binary");

  QCheckBox* smoothingCheckBox = new QCheckBox();
  smoothingCheckBox->setChecked(false);
  QLabel* smoothingLabel = new QLabel("Smoothing");

  QSpinBox* smoothingSigmaSpinBox = new QSpinBox();
  QLabel* smoothingSigmaLabel = new QLabel("Smoothing sigma");

  layout->addWidget(thresholdSpinBox,     0, 0);
  layout->addWidget(thresholdLabel,       0, 1);
  layout->addWidget(binaryCheckbox,       1, 0);
  layout->addWidget(binaryLabel,          1, 1);
  layout->addWidget(smoothingCheckBox,    2, 0);
  layout->addWidget(smoothingLabel,       2, 1);
  layout->addWidget(smoothingSigmaSpinBox,3, 0);
  layout->addWidget(smoothingSigmaLabel,  3, 1);

  return retval;
}

QWidget* SegmentationWidget::createCotourOptionsWidget()
{
  QWidget* retval = new QWidget(this);
  //TODO
  return retval;
}

//------------------------------------------------------------------------------

CenterlineWidget::CenterlineWidget(QWidget* parent) :
  WhatsThisWidget(parent), mFindCenterlineButton(new QPushButton("Find centerline"))
{
  this->setObjectName("CenterlineWidget");
  this->setWindowTitle("Centerline");

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mFindCenterlineButton);
  layout->addStretch();

  connect(mFindCenterlineButton, SIGNAL(clicked()), this, SLOT(findCenterlineSlot()));
}

CenterlineWidget::~CenterlineWidget()
{
}

QString CenterlineWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Centerline extraction.</h3>"
    "<p><i>Extract the centerline from a segment.</i></p>"
    "<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
    "</html>";
}

void CenterlineWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void CenterlineWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

void CenterlineWidget::findCenterlineSlot()
{
  //TODO Call some fancy centerline algorithm
  ssc::messageManager()->sendDebug("TODO: Find centerline not connected to algorithm yet!");
}
//------------------------------------------------------------------------------
}//namespace cx
