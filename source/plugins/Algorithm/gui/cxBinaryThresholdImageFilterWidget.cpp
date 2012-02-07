  #include "cxBinaryThresholdImageFilterWidget.h"

#include "sscTypeConversions.h"
#include "sscImage.h"
#include "sscDataManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxPatientData.h"
#include "cxDataInterface.h"
#include "cxContour.h"
#include "sscMesh.h"
#include "cxPatientService.h"
#include "cxTimedAlgorithmProgressBar.h"

namespace cx
{
BinaryThresholdImageFilterWidget::BinaryThresholdImageFilterWidget(QWidget* parent) :
  BaseWidget(parent, "BinaryThresholdImageFilterWidget", "Binary Threshold Image Filter"),
  mBinary(false),
  mSurface(true),
  mUseSmothing(false),
  mDefaultColor("red")//,
//  mStatusLabel(new QLabel(""))
{
	mSegmentationAlgorithm.reset(new BinaryThresholdImageFilter);
	mContourAlgorithm.reset(new Contour);

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  connect(mSegmentationAlgorithm.get(), SIGNAL(finished()), this, SLOT(handleFinishedSlot()));
  connect(mContourAlgorithm.get(), SIGNAL(finished()), this, SLOT(handleContourFinishedSlot()));

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

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mSegmentationAlgorithm);
	mTimedAlgorithmProgressBar->attach(mContourAlgorithm);

  topLayout->addWidget(segmentButton, 1,0);
  topLayout->addWidget(segmentationOptionsButton, 1,1);
  topLayout->addWidget(segmentationOptionsWidget, 2, 0, 1, 2);
  topLayout->addWidget(mTimedAlgorithmProgressBar);
//  topLayout->addWidget(mStatusLabel);
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

void BinaryThresholdImageFilterWidget::setImageInputSlot(QString value)
{
  mSelectedImage->setValue(value);
}

void BinaryThresholdImageFilterWidget::segmentSlot()
{
	patientService()->getThresholdPreview()->removePreview(this);

  QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();

  mSegmentationAlgorithm->setInput(mSelectedImage->getImage(), outputBasePath, mSegmentationThresholdAdapter->getValue(), mUseSmothing, mSmoothingSigmaAdapter->getValue());

//  mStatusLabel->setText("<font color=orange> Generating segmentation... Please wait!</font>\n");
}

void BinaryThresholdImageFilterWidget::handleFinishedSlot()
{
  ssc::ImagePtr segmentedImage = mSegmentationAlgorithm->getOutput();
  if(!segmentedImage)
    return;

  if (mSurface)
  	this->generateSurface();
//  mStatusLabel->setText("<font color=green> Done. </font>\n");
//  emit outputImageChanged(segmentedImage->getUid());
}

void BinaryThresholdImageFilterWidget::generateSurface()
{
  ssc::ImagePtr segmentedImage = mSegmentationAlgorithm->getOutput();
  if(!segmentedImage)
    return;

  QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();
  double decimation = 0.8;
  double threshold = 1;/// because the segmented image is 0..1
  bool reduceResolution = false;
  bool smoothing = true;

  mContourAlgorithm->setInput(segmentedImage, outputBasePath, threshold, decimation, reduceResolution, smoothing);
//  mStatusLabel->setText("<font color=orange> Generating contour... Please wait!</font>\n");
}

void BinaryThresholdImageFilterWidget::handleContourFinishedSlot()
{
  ssc::ImagePtr segmentedImage = mSegmentationAlgorithm->getOutput();
  if(!segmentedImage)
    return;

  ssc::MeshPtr outputMesh = mContourAlgorithm->getOutput();
  if(outputMesh)
  {
    outputMesh->setColor(mDefaultColor);
  }

//  mStatusLabel->setText("<font color=green> Done. </font>\n");

  emit outputImageChanged(segmentedImage->getUid());
}

void BinaryThresholdImageFilterWidget::toogleBinarySlot(bool on)
{
  mBinary = on;
}

void BinaryThresholdImageFilterWidget::thresholdSlot()
{
	patientService()->getThresholdPreview()->setPreview(this, mSelectedImage->getImage(),
			mSegmentationThresholdAdapter->getValue());
}

void BinaryThresholdImageFilterWidget::toogleSurfaceSlot(bool on)
{
  mSurface = on;
}

void BinaryThresholdImageFilterWidget::toogleSmoothingSlot(bool on)
{
  mUseSmothing = on;
  mSmoothingSigmaWidget->setEnabled(on);
}

void BinaryThresholdImageFilterWidget::imageChangedSlot(QString uid)
{
//	patientService()->getThresholdPreview()->removePreview(this);

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
//  QLabel* binaryLabel = new QLabel("Binary");
  connect(binaryCheckbox, SIGNAL(toggled(bool)), this, SLOT(toogleBinarySlot(bool)));

  QCheckBox* surfaceCheckBox = new QCheckBox();
  surfaceCheckBox->setChecked(mSurface);
  QLabel* surfaceLabel = new QLabel("Generate surface");
  connect(surfaceCheckBox, SIGNAL(toggled(bool)), this, SLOT(toogleSurfaceSlot(bool)));

  QCheckBox* smoothingCheckBox = new QCheckBox();
  smoothingCheckBox->setChecked(mUseSmothing);
  QLabel* smoothingLabel = new QLabel("Smoothing");
  connect(smoothingCheckBox, SIGNAL(toggled(bool)), this, SLOT(toogleSmoothingSlot(bool)));

  mSmoothingSigmaAdapter = ssc::DoubleDataAdapterXml::initialize("Smoothing sigma", "",
      "Used for smoothing the segmented volume",
      0.5, ssc::DoubleRange(0, 10, 0.1), 1);

  layout->addWidget(new ssc::SpinBoxAndSliderGroupWidget(this, mSegmentationThresholdAdapter));
  //We currently only have binary thresholding, so we remove this checkbox for now
//  QHBoxLayout* binaryLayout = new QHBoxLayout();
//  binaryLayout->addWidget(binaryCheckbox);
//  binaryLayout->addWidget(binaryLabel);
  QHBoxLayout* surfaceLayout = new QHBoxLayout();
  surfaceLayout->addWidget(surfaceCheckBox);
  surfaceLayout->addWidget(surfaceLabel);
  QHBoxLayout* smoothingLayout = new QHBoxLayout();
  smoothingLayout->addWidget(smoothingCheckBox);
  smoothingLayout->addWidget(smoothingLabel);
//  layout->addLayout(binaryLayout);
  layout->addLayout(surfaceLayout);
  layout->addLayout(smoothingLayout);

  mSmoothingSigmaWidget = ssc::SpinBoxAndSliderGroupWidgetPtr(new ssc::SpinBoxAndSliderGroupWidget(this, mSmoothingSigmaAdapter));
  mSmoothingSigmaWidget->setEnabled(smoothingCheckBox->isChecked());
  layout->addWidget(mSmoothingSigmaWidget.get());

  this->toogleBinarySlot(mBinary);
  this->thresholdSlot();
  this->toogleSurfaceSlot(mSurface);
  this->toogleSmoothingSlot(mUseSmothing);

  return retval;
}
//------------------------------------------------------------------------------

}//namespace cx
