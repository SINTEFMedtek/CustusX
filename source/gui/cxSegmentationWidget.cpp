#include "cxSegmentationWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QGridLayout>
#include <QCheckBox>

#include "sscTypeConversions.h"
#include "sscImage.h"
#include "sscDataManager.h"
#include "sscMessageManager.h"
#include "sscDoubleWidgets.h"
#include "cxDataInterface.h"
#include "cxVolumePropertiesWidget.h"
#include "cxSegmentation.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxFrameTreeWidget.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"

namespace cx
{

SegmentationWidget::SegmentationWidget(QWidget* parent) :
  WhatsThisWidget(parent),
  mSegmentationThreshold(100),
  mBinary(false),
  mUseSmothing(true),
  mSmoothSigma(0.5)
{
  this->setObjectName("SegmentationWidget");
  this->setWindowTitle("Segmentation");

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  mSelectedImage = SelectImageStringDataAdapter::New();
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(imageChanged(QString)));
  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);
  topLayout->addWidget(selectImageComboBox, 0, 0);

  QPushButton* segmentButton = new QPushButton("Segment", this);
  connect(segmentButton, SIGNAL(clicked()), this, SLOT(segmentSlot()));
  QPushButton* segmentationOptionsButton = new QPushButton("Options", this);
  segmentationOptionsButton->setCheckable(true);
  QGroupBox* segmentationOptionsGroupBox = this->createGroupBox(this->createSegmentationOptionsWidget(), new QWidget(), "Segmentation options");
  connect(segmentationOptionsButton, SIGNAL(clicked(bool)), segmentationOptionsGroupBox, SLOT(setVisible(bool)));
  connect(segmentationOptionsButton, SIGNAL(clicked()), this, SLOT(adjustSizeSlot()));
  segmentationOptionsGroupBox->setVisible(segmentationOptionsButton->isChecked());

  topLayout->addWidget(segmentationOptionsGroupBox, 1, 0, 1, 2);
  topLayout->addWidget(segmentButton, 2,0);
  topLayout->addWidget(segmentationOptionsButton, 2,1);

  this->adjustSizeSlot();
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

  if (ssc::dataManager()->getActiveImage())
  {
    mSelectedImage->setValue(qstring_cast(ssc::dataManager()->getActiveImage()->getUid()));
  }
}

void SegmentationWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

void SegmentationWidget::segmentSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();

  Segmentation().segment(mSelectedImage->getImage(), outputBasePath, mSegmentationThreshold, mUseSmothing, mSmoothSigma);
}

void SegmentationWidget::toogleBinarySlot(bool on)
{
  mBinary = on;
  ssc::messageManager()->sendWarning("The binary checkbox is not connected to anything yet.");
}

void SegmentationWidget::thresholdSlot(int value)
{
  mSegmentationThreshold = value;
}

void SegmentationWidget::toogleSmoothingSlot(bool on)
{
  mUseSmothing = on;

  mSmoothingSigmaSpinBox->setEnabled(on);
  mSmoothingSigmaLabel->setEnabled(on);
}

void SegmentationWidget::smoothingSigmaSlot(int value)
{
  mSmoothSigma = value;
}

QWidget* SegmentationWidget::createSegmentationOptionsWidget()
{
  QWidget* retval = new QWidget(this);

  QGridLayout* layout = new QGridLayout(retval);

  mSegmentationThresholdSpinBox = new QSpinBox();
  mSegmentationThresholdSpinBox->setValue(mSegmentationThreshold);
  QLabel* thresholdLabel = new QLabel("Threshold");
  connect(mSegmentationThresholdSpinBox, SIGNAL(valueChanged(int)), this, SLOT(thresholdSlot(int)));

  QCheckBox* binaryCheckbox = new QCheckBox();
  binaryCheckbox->setChecked(mBinary);
  binaryCheckbox->setChecked(false);
  QLabel* binaryLabel = new QLabel("Binary");
  connect(binaryCheckbox, SIGNAL(toggled(bool)), this, SLOT(toogleBinarySlot(bool)));

  QCheckBox* smoothingCheckBox = new QCheckBox();
  smoothingCheckBox->setChecked(mUseSmothing);
  smoothingCheckBox->setChecked(false);
  QLabel* smoothingLabel = new QLabel("Smoothing");
  connect(smoothingCheckBox, SIGNAL(toggled(bool)), this, SLOT(toogleSmoothingSlot(bool)));

  mSmoothingSigmaSpinBox = new QSpinBox();
  mSmoothingSigmaSpinBox->setValue(mSmoothSigma);
  mSmoothingSigmaSpinBox->setEnabled(smoothingCheckBox->isChecked());
  mSmoothingSigmaLabel = new QLabel("Smoothing sigma");
  mSmoothingSigmaLabel->setEnabled(smoothingCheckBox->isChecked());
  connect(mSmoothingSigmaSpinBox, SIGNAL(valueChanged(int)), this, SLOT(smoothingSigmaSlot(int)));

  layout->addWidget(mSegmentationThresholdSpinBox,      0, 0);
  layout->addWidget(thresholdLabel,                     0, 1);
  layout->addWidget(binaryCheckbox,                     1, 0);
  layout->addWidget(binaryLabel,                        1, 1);
  layout->addWidget(smoothingCheckBox,                  2, 0);
  layout->addWidget(smoothingLabel,                     2, 1);
  layout->addWidget(mSmoothingSigmaSpinBox,             3, 0);
  layout->addWidget(mSmoothingSigmaLabel,               3, 1);

  return retval;
}
//------------------------------------------------------------------------------

SurfaceWidget::SurfaceWidget(QWidget* parent) :
    WhatsThisWidget(parent), mSurfaceThreshold(100)
{
  this->setObjectName("SurfaceWidget");
  this->setWindowTitle("Surface");

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);

  mSelectedImage = SelectImageStringDataAdapter::New();
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(imageChanged(QString)));
  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);
  topLayout->addWidget(selectImageComboBox, 0, 0);


  QPushButton* surfaceButton = new QPushButton("Surface", this);
  connect(surfaceButton, SIGNAL(clicked()), this, SLOT(surfaceSlot()));
  QPushButton* surfaceOptionsButton = new QPushButton("Options", this);
  surfaceOptionsButton->setCheckable(true);
  QGroupBox* surfaceOptionsGroupBox = this->createGroupBox(this->createSurfaceOptionsWidget(), new QWidget(), "Surface options");
  connect(surfaceOptionsButton, SIGNAL(clicked(bool)), surfaceOptionsGroupBox, SLOT(setVisible(bool)));
  connect(surfaceOptionsButton, SIGNAL(clicked()), this, SLOT(adjustSizeSlot()));
  surfaceOptionsGroupBox->setVisible(surfaceOptionsButton->isChecked());

  topLayout->addWidget(surfaceOptionsGroupBox, 1, 0, 1, 2);
  topLayout->addWidget(surfaceButton, 2,0);
  topLayout->addWidget(surfaceOptionsButton,2,1);
}

SurfaceWidget::~SurfaceWidget()
{}

QString SurfaceWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Surfacing.</h3>"
    "<p><i>Find the surface of a binary volume using marching cubes.</i></p>"
    "</html>";
}

void SurfaceWidget::surfaceSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();

  Segmentation().contour(mSelectedImage->getImage(), outputBasePath, mSurfaceThreshold);
}

void SurfaceWidget::thresholdSlot(int value)
{
  mSurfaceThreshold = value;
}

QWidget* SurfaceWidget::createSurfaceOptionsWidget()
{
  QWidget* retval = new QWidget(this);
  QGridLayout* layout = new QGridLayout(retval);

  mSurfaceThresholdSpinBox = new QSpinBox();
  mSurfaceThresholdSpinBox->setValue(mSurfaceThreshold);
  QLabel* thresholdLabel = new QLabel("Threshold");
  connect(mSurfaceThresholdSpinBox, SIGNAL(valueChanged(int)), this, SLOT(thresholdSlot(int)));

  layout->addWidget(mSurfaceThresholdSpinBox,       0, 0);
  layout->addWidget(thresholdLabel,                 0, 1);

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

RegisterI2IWidget::RegisterI2IWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mRegisterButton(new QPushButton("Register")),
    mFixedImageLabel(new QLabel("<font color=\"green\">Fixed image: </font>")),
    mMovingImageLabel(new QLabel("<font color=\"blue\">Moving image: </font>"))
{
  QVBoxLayout* topLayout = new QVBoxLayout(this);
  QGridLayout* layout = new QGridLayout();
  topLayout->addLayout(layout);

  layout->addWidget(mFixedImageLabel, 0, 0);
  layout->addWidget(mMovingImageLabel, 1, 0);
  layout->addWidget(mRegisterButton, 2, 0);
  layout->addWidget(new QLabel("Parent frame tree status:"), 3, 0);
  layout->addWidget(new FrameTreeWidget(this), 4, 0);
}

RegisterI2IWidget::~RegisterI2IWidget()
{}

QString RegisterI2IWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Registration of vessel segments to eachother.</h3>"
      "<p><i>Press the button to perform vessel based registration between image 1 and image 2s centerlines.</i></p>"
      "</html>";
}

void RegisterI2IWidget::fixedImageSlot(const QString uid)
{
  mFixedImage = ssc::dataManager()->getImage(uid);
  if(!mFixedImage)
    return;
  mFixedImageLabel->setText(qstring_cast("<font color=\"green\"> Fixed image: <b>"+mFixedImage->getName()+"</b></font>"));
}

void RegisterI2IWidget::movingImageSlot(const QString uid)
{
  mMovingImage = ssc::dataManager()->getImage(uid);
  if(!mMovingImage)
    return;
  mMovingImageLabel->setText(qstring_cast("<font color=\"blue\">Moving image: <b>"+mMovingImage->getName()+"</b></font>"));
}

//------------------------------------------------------------------------------

}//namespace cx
