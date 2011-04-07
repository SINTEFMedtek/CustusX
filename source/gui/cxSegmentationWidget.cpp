  #include "cxSegmentationWidget.h"

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
#include "cxSegmentationOld.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxFrameTreeWidget.h"
#include "cxDataInterface.h"
#include "cxDataLocations.h"
#include "cxSeansVesselRegistrationWidget.h"

//Testing
#include "vesselReg/SeansVesselReg.hxx"

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

SegmentationWidget::SegmentationWidget(QWidget* parent) :
  WhatsThisWidget(parent, "SegmentationWidget", "Segmentation"),
  mSegmentationThreshold(100),
  mBinary(false),
  mUseSmothing(true),
  mSmoothSigma(0.5),
  mSegmentationThresholdSpinBox(new QSpinBox()),
  mSmoothingSigmaSpinBox(new QDoubleSpinBox()),
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
  connect(segmentationOptionsButton, SIGNAL(clicked()), this, SLOT(adjustSizeSlot()));
  segmentationOptionsWidget->setVisible(segmentationOptionsButton->isChecked());

  topLayout->addWidget(segmentButton, 1,0);
  topLayout->addWidget(segmentationOptionsButton, 1,1);
  topLayout->addWidget(segmentationOptionsWidget, 2, 0, 1, 2);
  topLayout->addWidget(mStatusLabel);
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

void SegmentationWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
  this->revertTransferFunctions();
}

void SegmentationWidget::setImageInputSlot(QString value)
{
  mSelectedImage->setValue(value);
}

void SegmentationWidget::segmentSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  this->revertTransferFunctions();

  mSegmentationAlgorithm.setInput(mSelectedImage->getImage(), outputBasePath, mSegmentationThreshold, mUseSmothing, mSmoothSigma);

  mStatusLabel->setText("<font color=orange> Generating segmentation... Please wait!</font>\n");
}

void SegmentationWidget::handleFinishedSlot()
{
  ssc::ImagePtr segmentedImage = mSegmentationAlgorithm.getOutput();
  if(!segmentedImage)
    return;

  mStatusLabel->setText("<font color=green> Done. </font>\n");

  emit outputImageChanged(segmentedImage->getUid());
}

void SegmentationWidget::toogleBinarySlot(bool on)
{
  mBinary = on;
}

void SegmentationWidget::revertTransferFunctions()
{
  if (!mModifiedImage)
    return;

  mModifiedImage->resetTransferFunction(mTF3D_original, mTF2D_original);

  mTF3D_original.reset();
  mTF2D_original.reset();
  mModifiedImage.reset();
}

void SegmentationWidget::thresholdSlot(int value)
{
  mSegmentationThreshold = value;

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
  tf3D->addAlphaPoint(value , 0);
  tf3D->addAlphaPoint(value+1, image->getMaxAlphaValue());
  tf3D->addColorPoint(value, Qt::green);
  tf3D->addColorPoint(image->getMax(), Qt::green);
}

void SegmentationWidget::toogleSmoothingSlot(bool on)
{
  mUseSmothing = on;

  mSmoothingSigmaSpinBox->setEnabled(on);
  mSmoothingSigmaLabel->setEnabled(on);
}

void SegmentationWidget::smoothingSigmaSlot(double value)
{
  mSmoothSigma = value;
}

void SegmentationWidget::imageChangedSlot(QString uid)
{
  this->revertTransferFunctions();

  ssc::ImagePtr image = ssc::dataManager()->getImage(uid);
  if(!image)
    return;
  mSegmentationThresholdSpinBox->setRange(image->getMin(), image->getMax());

  QString imageName = image->getName();
  if(imageName.contains("us", Qt::CaseInsensitive)) //assume the image is ultrasound
    this->toogleSmoothingSlot(true);
}

QWidget* SegmentationWidget::createSegmentationOptionsWidget()
{
  QWidget* retval = new QWidget(this);

  QGridLayout* layout = new QGridLayout(retval);

  mSegmentationThresholdSpinBox->setSingleStep(1);
  mSegmentationThresholdSpinBox->setValue(mSegmentationThreshold);
  QLabel* thresholdLabel = new QLabel("Threshold");
  connect(mSegmentationThresholdSpinBox, SIGNAL(valueChanged(int)), this, SLOT(thresholdSlot(int)));

  QCheckBox* binaryCheckbox = new QCheckBox();
  binaryCheckbox->setChecked(mBinary);
  binaryCheckbox->setChecked(true); // atm we only support binary thresholding
  binaryCheckbox->setEnabled(false); //TODO enable when the segmentation routine supports other than binary thresholding
  QLabel* binaryLabel = new QLabel("Binary");
  connect(binaryCheckbox, SIGNAL(toggled(bool)), this, SLOT(toogleBinarySlot(bool)));

  QCheckBox* smoothingCheckBox = new QCheckBox();
  smoothingCheckBox->setChecked(mUseSmothing);
  smoothingCheckBox->setChecked(false);
  QLabel* smoothingLabel = new QLabel("Smoothing");
  connect(smoothingCheckBox, SIGNAL(toggled(bool)), this, SLOT(toogleSmoothingSlot(bool)));

  mSmoothingSigmaSpinBox->setRange(0,10);
  mSmoothingSigmaSpinBox->setSingleStep(0.1);
  mSmoothingSigmaSpinBox->setValue(mSmoothSigma);
  mSmoothingSigmaSpinBox->setEnabled(smoothingCheckBox->isChecked());
  mSmoothingSigmaLabel = new QLabel("Smoothing sigma");
  mSmoothingSigmaLabel->setEnabled(smoothingCheckBox->isChecked());
  connect(mSmoothingSigmaSpinBox, SIGNAL(valueChanged(double)), this, SLOT(smoothingSigmaSlot(double)));

  layout->addWidget(mSegmentationThresholdSpinBox,      0, 0);
  layout->addWidget(thresholdLabel,                     0, 1);
  layout->addWidget(binaryCheckbox,                     1, 0);
  layout->addWidget(binaryLabel,                        1, 1);
  layout->addWidget(smoothingCheckBox,                  2, 0);
  layout->addWidget(smoothingLabel,                     2, 1);
  layout->addWidget(mSmoothingSigmaSpinBox,             3, 0);
  layout->addWidget(mSmoothingSigmaLabel,               3, 1);

  this->toogleBinarySlot(mBinary);
  this->thresholdSlot(mSegmentationThreshold);
  this->toogleSmoothingSlot(mUseSmothing);
  this->smoothingSigmaSlot(mSmoothSigma);

  return retval;
}
//------------------------------------------------------------------------------

SurfaceWidget::SurfaceWidget(QWidget* parent) :
    WhatsThisWidget(parent, "SurfaceWidget", "Surface"),
    mSurfaceThreshold(100),
    mDecimation(80),
    mReduceResolution(false),
    mSmoothing(true),
    mSurfaceThresholdSpinBox(new QSpinBox()),
    mDecimationSpinBox(new QSpinBox()),
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

  this->thresholdSlot(mSurfaceThreshold);
  this->decimationSlot(mDecimation);
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

  double decimation = mDecimation/100;

  mContourAlgorithm.setInput(mSelectedImage->getImage(), outputBasePath, mSurfaceThreshold, decimation, mReduceResolution, mSmoothing);

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

void SurfaceWidget::thresholdSlot(int value)
{
  mSurfaceThreshold = value;
}

void SurfaceWidget::decimationSlot(int value)
{
  mDecimation = value;
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
  mSurfaceThresholdSpinBox->setRange(image->getMin(), image->getMax());
}
void SurfaceWidget::setDefaultColor(QColor color)
{
  mDefaultColor = color;
}

QWidget* SurfaceWidget::createSurfaceOptionsWidget()
{
  QWidget* retval = new QWidget(this);
  QGridLayout* layout = new QGridLayout(retval);

  mSurfaceThresholdSpinBox->setSingleStep(1);
  mSurfaceThresholdSpinBox->setValue(mSurfaceThreshold);
  QLabel* thresholdLabel = new QLabel("Threshold");
  connect(mSurfaceThresholdSpinBox, SIGNAL(valueChanged(int)), this, SLOT(thresholdSlot(int)));

  mDecimationSpinBox->setRange(0,100);
  mDecimationSpinBox->setSingleStep(5);
  mDecimationSpinBox->setValue(mDecimation);
  QLabel* decimationLabel = new QLabel("Decimation %");
  connect(mDecimationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(decimationSlot(int)));

  QCheckBox* reduceResolutionCheckBox = new QCheckBox("Reduce input volumes resolution");
  reduceResolutionCheckBox->setChecked(mReduceResolution);
  connect(reduceResolutionCheckBox, SIGNAL(toggled(bool)), this, SLOT(reduceResolutionSlot(bool)));

  QCheckBox* smoothingCheckBox = new QCheckBox("Smoothing");
  smoothingCheckBox->setChecked(mSmoothing);
  connect(smoothingCheckBox, SIGNAL(toggled(bool)), this, SLOT(smoothingSlot(bool)));

  QLabel* inputLabel = new QLabel("Input:");
  QLabel* outputLabel = new QLabel("Output:");

  layout->addWidget(inputLabel,                     0, 0, 1, 2);
  layout->addWidget(mSurfaceThresholdSpinBox,       1, 0);
  layout->addWidget(thresholdLabel,                 1, 1);
  layout->addWidget(reduceResolutionCheckBox,       2, 0);
  layout->addWidget(outputLabel,                    3, 0, 1, 2);
  layout->addWidget(mDecimationSpinBox,             4, 0);
  layout->addWidget(decimationLabel,                4, 1);
  layout->addWidget(smoothingCheckBox,              5, 0);

  return retval;
}
//------------------------------------------------------------------------------

CenterlineWidget::CenterlineWidget(QWidget* parent) :
  WhatsThisWidget(parent, "CenterlineWidget", "CenterlineWidget"),
  mFindCenterlineButton(new QPushButton("Find centerline")),
  mDefaultColor("red"),
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
  mDefaultColor = color;
}

void CenterlineWidget::findCenterlineSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  mCenterlineAlgorithm.setInput(mSelectedImage->getImage(), outputBasePath);

  mStatusLabel->setText("<font color=orange> Generating centerline... Please wait!</font>\n");
}

void CenterlineWidget::handleFinishedSlot()
{
  ssc::ImagePtr centerlineImage = mCenterlineAlgorithm.getOutput();
  if(!centerlineImage)
    return;

  mStatusLabel->setText("<font color=green> Done. </font>\n");

  emit outputImageChanged(centerlineImage->getUid());
}

void CenterlineWidget::visualizeSlot(QString inputUid)
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();

  ssc::ImagePtr centerlineImage = ssc::dataManager()->getImage(inputUid);
  if(!centerlineImage)
    return;

  //automatically generate a mesh from the centerline
  vtkPolyDataPtr centerlinePolyData = SeansVesselReg::extractPolyData(centerlineImage, 1, 0);

  QString uid = centerlineImage->getUid() + "_ge%1";
  QString name = centerlineImage->getName() + " ge%1";
  ssc::MeshPtr mesh = ssc::dataManager()->createMesh(centerlinePolyData, uid, name, "Images");
  mesh->setColor(mDefaultColor);
  mesh->get_rMd_History()->setParentFrame(centerlineImage->getUid());
  ssc::dataManager()->loadData(mesh);
  ssc::dataManager()->saveMesh(mesh, outputBasePath);

  emit outputImageChanged(centerlineImage->getUid());
}

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
