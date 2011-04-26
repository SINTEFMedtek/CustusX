  #include "cxBinaryThresholdImageFilterWidget.h"

#include <boost/bind.hpp>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QGridLayout>
#include <QCheckBox>

#include <vtkPolyData.h>

#include "sscUtilHelpers.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscTypeConversions.h"
#include "sscImage.h"
#include "sscMesh.h"
#include "sscDataManager.h"
#include "sscMessageManager.h"
#include "sscDoubleWidgets.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxDataInterface.h"
#include "cxVolumePropertiesWidget.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxFrameTreeWidget.h"
#include "cxDataInterface.h"
#include "cxDataLocations.h"
#include "cxSeansVesselRegistrationWidget.h"


namespace cx
{
ResampleWidget::ResampleWidget(QWidget* parent) :
  WhatsThisWidget(parent, "ResampleWidget", "Resample"),
  mStatusLabel(new QLabel(""))
{
  connect(&mResampleAlgorithm, SIGNAL(finished()), this, SLOT(handleFinishedSlot()));

  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);

  mReferenceImage = SelectImageStringDataAdapter::New();
  mReferenceImage->setValueName("Select reference: ");
  ssc::LabeledComboBoxWidget* referenceImageComboBox = new ssc::LabeledComboBoxWidget(this, mReferenceImage);

  QPushButton* resampleButton = new QPushButton("Resample", this);
  connect(resampleButton, SIGNAL(clicked()), this, SLOT(resampleSlot()));

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  topLayout->addWidget(selectImageComboBox, 0, 0);
  topLayout->addWidget(referenceImageComboBox, 1, 0);
  topLayout->addWidget(mStatusLabel, 2, 0);
  topLayout->addWidget(resampleButton, 3, 0);
}

ResampleWidget::~ResampleWidget()
{
}

QString ResampleWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Resample.</h3>"
    "<p><i>Resample the volume into the space of the fixed volume. Crop to the same volume.</i></p>"
    "</html>";
}

void ResampleWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);

  if (ssc::dataManager()->getActiveImage())
  {
    mSelectedImage->setValue(qstring_cast(ssc::dataManager()->getActiveImage()->getUid()));
  }
}

void ResampleWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
}

void ResampleWidget::resampleSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  double margin = 20; //mm

  mResampleAlgorithm.setInput(mSelectedImage->getImage(), mReferenceImage->getImage(), outputBasePath, margin);
  mStatusLabel->setText("<font color=orange> Generating resampling... Please wait!</font>\n");
}

void ResampleWidget::handleFinishedSlot()
{
  ssc::ImagePtr output = mResampleAlgorithm.getOutput();
  if(!output)
    return;

  mStatusLabel->setText("<font color=green> Done. </font>\n");

  emit outputImageChanged(output->getUid());
}

QWidget* ResampleWidget::createOptionsWidget()
{
  QWidget* retval = new QWidget(this);
  return retval;
}
//------------------------------------------------------------------------------

BinaryThresholdImageFilterWidget::BinaryThresholdImageFilterWidget(QWidget* parent) :
  WhatsThisWidget(parent, "BinaryThresholdImageFilterWidget", "Binary Threshold Image Filter"),
  mBinary(false),
  mUseSmothing(false),
  mStatusLabel(new QLabel(""))
{
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  connect(&mSegmentationAlgorithm, SIGNAL(finished()), this, SLOT(handleFinishedSlot()));

  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SLOT(imageChangedSlot(QString)));

  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);
  topLayout->addWidget(selectImageComboBox, 0, 0);

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

BinaryThresholdImageFilterWidget::~BinaryThresholdImageFilterWidget()
{
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
  }
  image->resetTransferFunctions();
  ssc::ImageTFDataPtr tf3D = image->getTransferFunctions3D();
  tf3D->addAlphaPoint(mSegmentationThresholdAdapter->getValue() , 0);
  tf3D->addAlphaPoint(mSegmentationThresholdAdapter->getValue()+1, image->getMaxAlphaValue());
  tf3D->addColorPoint(mSegmentationThresholdAdapter->getValue(), Qt::green);
  tf3D->addColorPoint(image->getMax(), Qt::green);
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

  mSegmentationThresholdAdapter->setValueRange(ssc::DoubleRange(image->getMin(), image->getMax(), 1));

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

SurfaceWidget::SurfaceWidget(QWidget* parent) :
    WhatsThisWidget(parent, "SurfaceWidget", "Surface"),
    mReduceResolution(false),
    mSmoothing(true),
    mDefaultColor("red"),
    mStatusLabel(new QLabel(""))
{
  connect(&mContourAlgorithm, SIGNAL(finished()), this, SLOT(handleFinishedSlot()));

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SLOT(imageChangedSlot(QString)));
  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);
  topLayout->addWidget(selectImageComboBox, 0, 0);

  QPushButton* surfaceButton = new QPushButton("Surface", this);
  connect(surfaceButton, SIGNAL(clicked()), this, SLOT(surfaceSlot()));
  QPushButton* surfaceOptionsButton = new QPushButton("Options", this);
  surfaceOptionsButton->setCheckable(true);
  QGroupBox* surfaceOptionsWidget = this->createGroupbox(this->createSurfaceOptionsWidget(), "Surface options");
  connect(surfaceOptionsButton, SIGNAL(clicked(bool)), surfaceOptionsWidget, SLOT(setVisible(bool)));
  connect(surfaceOptionsButton, SIGNAL(clicked()), this, SLOT(adjustSizeSlot()));
  surfaceOptionsWidget->setVisible(surfaceOptionsButton->isChecked());

  topLayout->addWidget(surfaceButton, 1,0);
  topLayout->addWidget(surfaceOptionsButton,1,1);
  topLayout->addWidget(surfaceOptionsWidget, 2, 0, 1, 2);
  topLayout->addWidget(mStatusLabel);

  this->adjustSizeSlot();
  this->reduceResolutionSlot(mReduceResolution);
  this->smoothingSlot(mSmoothing);
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

void SurfaceWidget::setImageInputSlot(QString value)
{
  mSelectedImage->setValue(value);
}

void SurfaceWidget::surfaceSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  double decimation = mDecimationAdapter->getValue()/100;

  mContourAlgorithm.setInput(mSelectedImage->getImage(), outputBasePath, mSurfaceThresholdAdapter->getValue(), decimation, mReduceResolution, mSmoothing);

  mStatusLabel->setText("<font color=orange> Generating contour... Please wait!</font>\n");
}

void SurfaceWidget::handleFinishedSlot()
{
  ssc::MeshPtr outputMesh = mContourAlgorithm.getOutput();
  if(!outputMesh)
    return;

  outputMesh->setColor(mDefaultColor);
  mStatusLabel->setText("<font color=green> Done. </font>\n");

  emit outputMeshChanged(outputMesh->getUid());
}

void SurfaceWidget::reduceResolutionSlot(bool value)
{
  mReduceResolution = value;
}

void SurfaceWidget::smoothingSlot(bool value)
{
  mSmoothing = value;
}
void SurfaceWidget::imageChangedSlot(QString uid)
{
  ssc::ImagePtr image = ssc::dataManager()->getImage(uid);
  if(!image)
    return;
  mSurfaceThresholdAdapter->setValueRange(ssc::DoubleRange(image->getMin(), image->getMax(), 1));
  mSurfaceThresholdAdapter->setValue(image->getRange() / 2 + image->getMin());
}
void SurfaceWidget::setDefaultColor(QColor color)
{
  mDefaultColor = color;
}

QWidget* SurfaceWidget::createSurfaceOptionsWidget()
{
  QWidget* retval = new QWidget(this);
  QVBoxLayout* layout = new QVBoxLayout(retval);

  mSurfaceThresholdAdapter = ssc::DoubleDataAdapterXml::initialize("Threshold", "",
      "Values from this threshold and above will be included",
      100.0, ssc::DoubleRange(-1000, 1000, 1), 0);
  mDecimationAdapter = ssc::DoubleDataAdapterXml::initialize("Decimation %", "",
      "Reduce number of triangles in output surface",
      80.0, ssc::DoubleRange(0, 100, 1), 0);

  QCheckBox* reduceResolutionCheckBox = new QCheckBox("Reduce input volumes resolution");
  reduceResolutionCheckBox->setChecked(mReduceResolution);
  connect(reduceResolutionCheckBox, SIGNAL(toggled(bool)), this, SLOT(reduceResolutionSlot(bool)));

  QCheckBox* smoothingCheckBox = new QCheckBox("Smoothing");
  smoothingCheckBox->setChecked(mSmoothing);
  connect(smoothingCheckBox, SIGNAL(toggled(bool)), this, SLOT(smoothingSlot(bool)));

  QLabel* inputLabel = new QLabel("Input:");
  QLabel* outputLabel = new QLabel("Output:");

  layout->addWidget(inputLabel);
  layout->addWidget(new ssc::SpinBoxAndSliderGroupWidget(this, mSurfaceThresholdAdapter));
  layout->addWidget(reduceResolutionCheckBox);
  layout->addWidget(outputLabel);
  layout->addWidget(new ssc::SpinBoxAndSliderGroupWidget(this, mDecimationAdapter));
  layout->addWidget(smoothingCheckBox);

  return retval;
}
//------------------------------------------------------------------------------

CenterlineWidget::CenterlineWidget(QWidget* parent) :
  WhatsThisWidget(parent, "CenterlineWidget", "CenterlineWidget"),
  mFindCenterlineButton(new QPushButton("Find centerline")),
//  mDefaultColor("red"),
  mStatusLabel(new QLabel(""))
{
  connect(&mCenterlineAlgorithm, SIGNAL(finished()), this, SLOT(handleFinishedSlot()));

  QVBoxLayout* layout = new QVBoxLayout(this);

  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);

  layout->addWidget(selectImageComboBox);
  layout->addWidget(mFindCenterlineButton);
  layout->addWidget(mStatusLabel);
  layout->addStretch();

  connect(mFindCenterlineButton, SIGNAL(clicked()), this, SLOT(findCenterlineSlot()));
}

CenterlineWidget::~CenterlineWidget()
{}

QString CenterlineWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Centerline extraction.</h3>"
    "<p><i>Extract the centerline from a segment.</i></p>"
    "<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
    "</html>";
}

void CenterlineWidget::setImageInputSlot(QString value)
{
  mSelectedImage->setValue(value);
}

void CenterlineWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void CenterlineWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

void CenterlineWidget::setDefaultColor(QColor color)
{
  mCenterlineAlgorithm.setDefaultColor(color);
//  mDefaultColor = color;
}

void CenterlineWidget::findCenterlineSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  mCenterlineAlgorithm.setInput(mSelectedImage->getImage(), outputBasePath);

  mStatusLabel->setText("<font color=orange> Generating centerline... Please wait!</font>\n");
}

void CenterlineWidget::handleFinishedSlot()
{
  ssc::DataPtr centerlineImage = mCenterlineAlgorithm.getOutput();
  if(!centerlineImage)
    return;

  mStatusLabel->setText("<font color=green> Done. </font>\n");

  emit outputImageChanged(centerlineImage->getUid());
}

//void CenterlineWidget::visualizeSlot(QString inputUid)
//{
//  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
//
//  ssc::ImagePtr centerlineImage = ssc::dataManager()->getImage(inputUid);
//  if(!centerlineImage)
//    return;
//
//  //automatically generate a mesh from the centerline
//  vtkPolyDataPtr centerlinePolyData = SeansVesselReg::extractPolyData(centerlineImage, 1, 0);
//
//  QString uid = centerlineImage->getUid() + "_ge%1";
//  QString name = centerlineImage->getName() + " ge%1";
//  ssc::MeshPtr mesh = ssc::dataManager()->createMesh(centerlinePolyData, uid, name, "Images");
//  mesh->setColor(mDefaultColor);
//  mesh->get_rMd_History()->setParentFrame(centerlineImage->getUid());
//  ssc::dataManager()->loadData(mesh);
//  ssc::dataManager()->saveMesh(mesh, outputBasePath);
//
//  emit outputImageChanged(centerlineImage->getUid());
//}

//------------------------------------------------------------------------------

RegisterI2IWidget::RegisterI2IWidget(QWidget* parent) :
    WhatsThisWidget(parent, "RegisterI2IWidget", "Register Image2Image"),
    mSeansVesselRegsitrationWidget(new SeansVesselRegistrationWidget(this))
{
  connect(registrationManager(), SIGNAL(fixedDataChanged(QString)), this, SLOT(fixedImageSlot(QString)));
  connect(registrationManager(), SIGNAL(movingDataChanged(QString)), this, SLOT(movingImageSlot(QString)));

  QVBoxLayout* topLayout = new QVBoxLayout(this);
  QGridLayout* layout = new QGridLayout();
  topLayout->addLayout(layout);

  layout->addWidget(mSeansVesselRegsitrationWidget);
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

void RegisterI2IWidget::fixedImageSlot(QString uid)
{
  mSeansVesselRegsitrationWidget->fixedImageSlot(uid);
}

void RegisterI2IWidget::movingImageSlot(QString uid)
{
  mSeansVesselRegsitrationWidget->movingImageSlot(uid);
}

//------------------------------------------------------------------------------

}//namespace cx
