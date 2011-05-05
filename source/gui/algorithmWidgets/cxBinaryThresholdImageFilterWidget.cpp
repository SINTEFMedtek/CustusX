  #include "cxBinaryThresholdImageFilterWidget.h"

#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscTypeConversions.h"
#include "sscImage.h"
#include "sscDataManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxDataInterface.h"
#include "cxContour.h"
#include "sscMesh.h"

namespace cx
{
BinaryThresholdImageFilterWidget::BinaryThresholdImageFilterWidget(QWidget* parent) :
  BaseWidget(parent, "BinaryThresholdImageFilterWidget", "Binary Threshold Image Filter"),
  mBinary(false),
  mUseSmothing(false),
  mDefaultColor("red"),
  mStatusLabel(new QLabel(""))
{
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  connect(&mSegmentationAlgorithm, SIGNAL(finished()), this, SLOT(handleFinishedSlot()));
  connect(&mContourAlgorithm, SIGNAL(finished()), this, SLOT(handleContourFinishedSlot()));

  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SLOT(imageChangedSlot(QString)));

  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);
  topLayout->addWidget(selectImageComboBox, 0, 0, 1, 2);

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

  //Timer for removing segmentation preview coloring if widget is not visible
  mRemoveTimer = new QTimer(this);
  connect(mRemoveTimer, SIGNAL(timeout()), this, SLOT(removeIfNotVisible()));
}

BinaryThresholdImageFilterWidget::~BinaryThresholdImageFilterWidget()
{
}

void BinaryThresholdImageFilterWidget::setDefaultColor(QColor color)
{
  mDefaultColor = color;
}

QString BinaryThresholdImageFilterWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Binary Threshold Image Filter.</h3>"
    "<p><i>Segment out areas from the selected image using a threshold.</i></p>"
    "</html>";
}

void BinaryThresholdImageFilterWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);

  if (ssc::dataManager()->getActiveImage())
  {
    mSelectedImage->setValue(qstring_cast(ssc::dataManager()->getActiveImage()->getUid()));
  }
}

void BinaryThresholdImageFilterWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
  this->revertTransferFunctions();
}

void BinaryThresholdImageFilterWidget::removeIfNotVisible()
{
  //Revert to original transfer functions when the widget is no longer visible
  if(this->visibleRegion().isEmpty())
  {
    mRemoveTimer->stop();
    this->revertTransferFunctions();
  }
}

void BinaryThresholdImageFilterWidget::setImageInputSlot(QString value)
{
  mSelectedImage->setValue(value);
}

void BinaryThresholdImageFilterWidget::segmentSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  this->revertTransferFunctions();

  mSegmentationAlgorithm.setInput(mSelectedImage->getImage(), outputBasePath, mSegmentationThresholdAdapter->getValue(), mUseSmothing, mSmoothingSigmaAdapter->getValue());

  mStatusLabel->setText("<font color=orange> Generating segmentation... Please wait!</font>\n");
}

void BinaryThresholdImageFilterWidget::handleFinishedSlot()
{
  ssc::ImagePtr segmentedImage = mSegmentationAlgorithm.getOutput();
  if(!segmentedImage)
    return;

  this->generateSurface();
//  mStatusLabel->setText("<font color=green> Done. </font>\n");
//  emit outputImageChanged(segmentedImage->getUid());
}

void BinaryThresholdImageFilterWidget::generateSurface()
{
  ssc::ImagePtr segmentedImage = mSegmentationAlgorithm.getOutput();
  if(!segmentedImage)
    return;

  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  double decimation = 0.8;
  double threshold = 1;/// because the segmented image is 0..1
  bool reduceResolution = false;
  bool smoothing = true;

  mContourAlgorithm.setInput(segmentedImage, outputBasePath, threshold, decimation, reduceResolution, smoothing);
//  ssc::MeshPtr outputMesh = mContourAlgorithm.getOutput();
//  if(outputMesh)
//  {
//    outputMesh->setColor(mDefaultColor);
//  }

  mStatusLabel->setText("<font color=orange> Generating contour... Please wait!</font>\n");
}

void BinaryThresholdImageFilterWidget::handleContourFinishedSlot()
{
  ssc::ImagePtr segmentedImage = mSegmentationAlgorithm.getOutput();
  if(!segmentedImage)
    return;

  ssc::MeshPtr outputMesh = mContourAlgorithm.getOutput();
  if(outputMesh)
  {
    outputMesh->setColor(mDefaultColor);
  }

  mStatusLabel->setText("<font color=green> Done. </font>\n");

  emit outputImageChanged(segmentedImage->getUid());
}

void BinaryThresholdImageFilterWidget::toogleBinarySlot(bool on)
{
  mBinary = on;
}

void BinaryThresholdImageFilterWidget::revertTransferFunctions()
{
  if (!mModifiedImage)
    return;

  mModifiedImage->resetTransferFunction(mTF3D_original, mTF2D_original);
  mModifiedImage->setShadingOn(mShadingOn_original);

  mTF3D_original.reset();
  mTF2D_original.reset();
  mModifiedImage.reset();
}

void BinaryThresholdImageFilterWidget::thresholdSlot()
{
  ssc::ImagePtr image = mSelectedImage->getImage();
  if(!image)
    return;

  if (!mModifiedImage)
  {
    mModifiedImage = image;
    mTF3D_original = image->getTransferFunctions3D()->createCopy();
    mTF2D_original = image->getLookupTable2D()->createCopy();
    mShadingOn_original = image->getShadingOn();
  }
  image->resetTransferFunctions();
  ssc::ImageTF3DPtr tf3D = image->getTransferFunctions3D();
  tf3D->removeInitAlphaPoint();
  tf3D->addAlphaPoint(mSegmentationThresholdAdapter->getValue()-1 , 0);
  tf3D->addAlphaPoint(mSegmentationThresholdAdapter->getValue(), image->getMaxAlphaValue());
  tf3D->addColorPoint(mSegmentationThresholdAdapter->getValue(), Qt::green);
  tf3D->addColorPoint(image->getMax(), Qt::green);
  image->setShadingOn(true);

  ssc::ImageLUT2DPtr lut2D = image->getLookupTable2D();
  lut2D->setFullRangeWinLevel();
  lut2D->addColorPoint(mSegmentationThresholdAdapter->getValue(), Qt::green);
  lut2D->addColorPoint(image->getMax(), Qt::green);
  lut2D->setLLR(mSegmentationThresholdAdapter->getValue());

  //Start timer that reverts transfer functions when widget is no longer visible
  mRemoveTimer->start(1000);
}

void BinaryThresholdImageFilterWidget::toogleSmoothingSlot(bool on)
{
  mUseSmothing = on;
  mSmoothingSigmaWidget->setEnabled(on);
}

void BinaryThresholdImageFilterWidget::imageChangedSlot(QString uid)
{
  this->revertTransferFunctions();

  ssc::ImagePtr image = ssc::dataManager()->getImage(uid);
  if(!image)
    return;
  mSegmentationThresholdAdapter->setValueRange(ssc::DoubleRange(image->getMin(), image->getMax(), 1));
  int initValue = image->getMin() + ((image->getMax() - image->getMin()) / 10);
  mSegmentationThresholdAdapter->setValue(initValue);

  QString imageName = image->getName();
  if(imageName.contains("us", Qt::CaseInsensitive)) //assume the image is ultrasound
    this->toogleSmoothingSlot(true);
}

QWidget* BinaryThresholdImageFilterWidget::createSegmentationOptionsWidget()
{
  QWidget* retval = new QWidget(this);

  QVBoxLayout* layout = new QVBoxLayout(retval);

  mSegmentationThresholdAdapter = ssc::DoubleDataAdapterXml::initialize("Threshold", "",
      "Values from this threshold and above will be included",
      100.0, ssc::DoubleRange(-1000, 1000, 1), 0);
  connect(mSegmentationThresholdAdapter.get(), SIGNAL(valueWasSet()),this, SLOT(thresholdSlot()));

  QCheckBox* binaryCheckbox = new QCheckBox();
  binaryCheckbox->setChecked(mBinary);
  binaryCheckbox->setChecked(true); // atm we only support binary thresholding
  binaryCheckbox->setEnabled(false); //TODO enable when the segmentation routine supports other than binary thresholding
  QLabel* binaryLabel = new QLabel("Binary");
  connect(binaryCheckbox, SIGNAL(toggled(bool)), this, SLOT(toogleBinarySlot(bool)));

  QCheckBox* smoothingCheckBox = new QCheckBox();
  smoothingCheckBox->setChecked(mUseSmothing);
  QLabel* smoothingLabel = new QLabel("Smoothing");
  connect(smoothingCheckBox, SIGNAL(toggled(bool)), this, SLOT(toogleSmoothingSlot(bool)));

  mSmoothingSigmaAdapter = ssc::DoubleDataAdapterXml::initialize("Smoothing sigma", "",
      "Used for smoothing the segmented volume",
      0.5, ssc::DoubleRange(0, 10, 0.1), 1);

  layout->addWidget(new ssc::SpinBoxAndSliderGroupWidget(this, mSegmentationThresholdAdapter));
  QHBoxLayout* binaryLayout = new QHBoxLayout();
  binaryLayout->addWidget(binaryCheckbox);
  binaryLayout->addWidget(binaryLabel);
  QHBoxLayout* smoothingLayout = new QHBoxLayout();
  smoothingLayout->addWidget(smoothingCheckBox);
  smoothingLayout->addWidget(smoothingLabel);
  layout->addLayout(binaryLayout);
  layout->addLayout(smoothingLayout);

  mSmoothingSigmaWidget = ssc::SpinBoxAndSliderGroupWidgetPtr(new ssc::SpinBoxAndSliderGroupWidget(this, mSmoothingSigmaAdapter));
  mSmoothingSigmaWidget->setEnabled(smoothingCheckBox->isChecked());
  layout->addWidget(mSmoothingSigmaWidget.get());

  this->toogleBinarySlot(mBinary);
  this->thresholdSlot();
  this->toogleSmoothingSlot(mUseSmothing);

  return retval;
}
//------------------------------------------------------------------------------

}//namespace cx
