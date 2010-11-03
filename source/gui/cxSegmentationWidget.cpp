#include "cxSegmentationWidget.h"

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
#include "cxDataInterface.h"
#include "cxVolumePropertiesWidget.h"
#include "cxSegmentation.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxFrameTreeWidget.h"
#include "cxDataInterface.h"
#include "cxDataLocations.h"
#include "sscLabeledComboBoxWidget.h"

//Testing
#include "vesselReg/SeansVesselReg.hxx"

namespace cx
{

SegmentationWidget::SegmentationWidget(QWidget* parent) :
  WhatsThisWidget(parent),
  mSegmentationThreshold(100),
  mBinary(false),
  mUseSmothing(true),
  mSmoothSigma(0.5),
  mSegmentationThresholdSpinBox(new QSpinBox()),
  mSmoothingSigmaSpinBox(new QDoubleSpinBox())
{
  this->setObjectName("SegmentationWidget");
  this->setWindowTitle("Segmentation");

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();


  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SLOT(imageChangedSlot(QString)));
  //connect(mSelectedImage.get(), SIGNAL(imageChanged()), this, SLOT(revertTransferFunctions()));
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

void SegmentationWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
  this->revertTransferFunctions();
}

void SegmentationWidget::segmentSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  this->revertTransferFunctions();

  ssc::ImagePtr segmentedImage = Segmentation().segment(mSelectedImage->getImage(), outputBasePath, mSegmentationThreshold, mUseSmothing, mSmoothSigma);
  if(!segmentedImage)
    return;
  emit outputImageChanged(segmentedImage->getUid());
}

void SegmentationWidget::toogleBinarySlot(bool on)
{
  mBinary = on;
  ssc::messageManager()->sendDebug("The binary checkbox is not connected to anything yet.");
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
  ssc::messageManager()->sendDebug("Segmentation threshold: "+qstring_cast(mSegmentationThreshold));

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
  ssc::ImageTF3DPtr tf3D = image->getTransferFunctions3D();
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

  ssc::messageManager()->sendDebug("Smoothing: "+qstring_cast(mUseSmothing));
}

void SegmentationWidget::smoothingSigmaSlot(double value)
{
  mSmoothSigma = value;
  ssc::messageManager()->sendDebug("Smoothing sigma: "+qstring_cast(mSmoothSigma));
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
  binaryCheckbox->setChecked(false);
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
    WhatsThisWidget(parent),
    mSurfaceThreshold(100),
    mDecimation(80),
    mReduceResolution(false),
    mSmoothing(true),
    mSurfaceThresholdSpinBox(new QSpinBox()),
    mDecimationSpinBox(new QSpinBox())

{
  this->setObjectName("SurfaceWidget");
  this->setWindowTitle("Surface");

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
  ssc::MeshPtr outputMesh = Segmentation().contour(mSelectedImage->getImage(), outputBasePath, mSurfaceThreshold, decimation, mReduceResolution, mSmoothing);
  if(!outputMesh)
    return;
  emit outputMeshChanged(outputMesh->getUid());
}

void SurfaceWidget::thresholdSlot(int value)
{
  mSurfaceThreshold = value;
}

void SurfaceWidget::decimationSlot(int value)
{
  mDecimation = value;
  ssc::messageManager()->sendDebug("Surface, decimation: "+qstring_cast(mDecimation));
}

void SurfaceWidget::reduceResolutionSlot(bool value)
{
  mReduceResolution = value;
  ssc::messageManager()->sendDebug("Surface, reduce resolution: "+qstring_cast(mReduceResolution));
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
  WhatsThisWidget(parent), mFindCenterlineButton(new QPushButton("Find centerline"))
{
  this->setObjectName("CenterlineWidget");
  this->setWindowTitle("Centerline");

  QVBoxLayout* layout = new QVBoxLayout(this);

  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);

  layout->addWidget(selectImageComboBox);
  layout->addWidget(mFindCenterlineButton);
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

void CenterlineWidget::findCenterlineSlot()
{
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  ssc::ImagePtr centerlineImage = Segmentation().centerline(mSelectedImage->getImage(), outputBasePath);
  if(!centerlineImage)
    return;

  std::cout << "centerline i bb " << centerlineImage->boundingBox() << std::endl;

  //automatically generate a mesh from the centerline
  vtkPolyDataPtr centerlinePolyData = SeansVesselReg::extractPolyData(centerlineImage, 1, 0);
  std::cout << "centerline p bb " << ssc::DoubleBoundingBox3D(centerlinePolyData->GetBounds()) << std::endl;

  QString uid = ssc::changeExtension(centerlineImage->getUid(), "") + "_mesh%1";
  QString name = centerlineImage->getName() + " mesh %1";
  ssc::MeshPtr mesh = ssc::dataManager()->createMesh(centerlinePolyData, uid, name, "Images");
  ssc::dataManager()->loadData(mesh);
  ssc::dataManager()->saveMesh(mesh, outputBasePath);

  emit outputImageChanged(centerlineImage->getUid());
}
//------------------------------------------------------------------------------

RegisterI2IWidget::RegisterI2IWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mRegisterButton(new QPushButton("Register")),
    mTestButton(new QPushButton("TEST, loads two minc images into the system.")),
    mFixedImageLabel(new QLabel("<font color=\"green\">Fixed image: </font>")),
    mMovingImageLabel(new QLabel("<font color=\"blue\">Moving image: </font>"))
{
  connect(registrationManager(), SIGNAL(fixedDataChanged(QString)), this, SLOT(fixedImageSlot(QString)));
  connect(registrationManager(), SIGNAL(movingDataChanged(QString)), this, SLOT(movingImageSlot(QString)));

  connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

  QVBoxLayout* topLayout = new QVBoxLayout(this);
  QGridLayout* layout = new QGridLayout();
  topLayout->addLayout(layout);

  layout->addWidget(mFixedImageLabel, 0, 0);
  layout->addWidget(mMovingImageLabel, 1, 0);
  layout->addWidget(mRegisterButton, 2, 0);
  layout->addWidget(new QLabel("Parent frame tree status:"), 3, 0);
  layout->addWidget(new FrameTreeWidget(this), 4, 0);

  //TESTING
  layout->addWidget(this->createHorizontalLine(), 5, 0);
  layout->addWidget(mTestButton, 6, 0);
  connect(mTestButton, SIGNAL(clicked()), this, SLOT(testSlot()));
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
  ssc::DataPtr fixedImage = registrationManager()->getFixedData();
  if(!fixedImage)
    return;
  mFixedImageLabel->setText(qstring_cast("<font color=\"green\"> Fixed data: <b>"+fixedImage->getName()+"</b></font>"));
  mFixedImageLabel->update();
}

void RegisterI2IWidget::movingImageSlot(QString uid)
{
  ssc::DataPtr movingImage = registrationManager()->getMovingData();
  if(!movingImage)
    return;
  mMovingImageLabel->setText(qstring_cast("<font color=\"blue\">Moving data: <b>"+movingImage->getName()+"</b></font>"));
  mMovingImageLabel->update();
}

void RegisterI2IWidget::testSlot()
{
  if(!stateManager()->getPatientData()->isPatientValid())
  {
    ssc::messageManager()->sendWarning("Create a new patient before trying to import the minc data.");
    return;
  }

  ssc::messageManager()->sendDebug("===============TESTING BUTTON START==============");

  int lts_ratio = 80;
  double stop_delta = 0.001;
  double lambda = 0;
  double sigma = 1.0;
  bool lin_flag = 1;
  int sample = 1;
  int single_point_thre = 1;
  bool verbose = 1;

  SeansVesselReg* theThing = new SeansVesselReg(lts_ratio,
        stop_delta,
        lambda,
        sigma,
        lin_flag,
        sample,
        single_point_thre,
        verbose);

  QString sourcefile(cx::DataLocations::getTestDataPath()+"/Nevro/IngeridCenterline/center_dim_110555_USA_blur.mnc");
  if(QFile::exists(sourcefile))
    ssc::messageManager()->sendInfo(sourcefile+" exists");
  else
  {
    QFile q_sourcefile(sourcefile);
    QFileInfo info(q_sourcefile);
    ssc::messageManager()->sendDebug(info.absoluteFilePath());
  }

  QString targetfile(cx::DataLocations::getTestDataPath()+"/Nevro/IngeridCenterline/center_dim_MRA_masked_like_110555USA.mnc");
  if(QFile::exists(targetfile))
    ssc::messageManager()->sendInfo(targetfile+" exsits");
  else
  {
    QFile q_targetfile(targetfile);
    QFileInfo info(q_targetfile);
    ssc::messageManager()->sendDebug(info.absoluteFilePath());
  }

  //read minc files and add them to the datamanager
  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  ssc::ImagePtr source = theThing->loadMinc(cstring_cast(QString(sourcefile)));
  ssc::dataManager()->loadData(source);
  ssc::dataManager()->saveImage(source, outputBasePath);
  ssc::ImagePtr target = theThing->loadMinc(cstring_cast(QString(targetfile)));
  ssc::dataManager()->loadData(target);
  ssc::dataManager()->saveImage(target, outputBasePath);

  vtkPolyDataPtr sourcePolyData = SeansVesselReg::extractPolyData(source, single_point_thre, 0);
  QString uid = ssc::changeExtension(source->getUid(), "") + "_mesh%1";
  QString name = source->getName() + " mesh %1";
  ssc::MeshPtr mesh = ssc::dataManager()->createMesh(sourcePolyData, uid, name, "Images");
  ssc::dataManager()->loadData(mesh);
  ssc::dataManager()->saveMesh(mesh, outputBasePath);

  ssc::messageManager()->sendDebug("===============TESTING BUTTON END==============");
}

void RegisterI2IWidget::registerSlot()
{
  registrationManager()->doVesselRegistration();
}

//------------------------------------------------------------------------------

}//namespace cx
