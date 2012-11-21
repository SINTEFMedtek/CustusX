/*
 * cxSurfaceWidget.cpp
 *
 *  \date Apr 29, 2011
 *      \author christiana
 */

#include <cxSurfaceWidget.h>

#include "sscImage.h"
#include "sscMesh.h"
#include "sscDataManager.h"
#include "sscDoubleWidgets.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscTypeConversions.h"
#include "cxPatientData.h"
#include "cxDataInterface.h"
#include "cxPatientService.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxRepManager.h"

namespace cx
{

SurfaceWidget::SurfaceWidget(QWidget* parent) :
    BaseWidget(parent, "SurfaceWidget", "Surface"),
    mReduceResolution(false),
    mSmoothing(true),
    mPreserveTopology(true),
    mDefaultColor("red")
//    mStatusLabel(new QLabel(""))
{
	mContourAlgorithm.reset(new Contour());
  connect(mContourAlgorithm.get(), SIGNAL(finished()), this, SLOT(handleFinishedSlot()));
  connect(mContourAlgorithm.get(), SIGNAL(aboutToStart()), this, SLOT(preprocessContour()));

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  // Create this early to make sure it is available when we assign tooltip values
  mReduceResolutionCheckBox = new QCheckBox("Reduce input volumes resolution");

  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(dataChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  connect(mSelectedImage.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
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

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mContourAlgorithm);

  topLayout->addWidget(surfaceButton, 1,0);
  topLayout->addWidget(surfaceOptionsButton,1,1);
  topLayout->addWidget(surfaceOptionsWidget, 2, 0, 1, 2);
//  topLayout->addWidget(mStatusLabel);
  topLayout->addWidget(mTimedAlgorithmProgressBar);

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


void SurfaceWidget::preprocessContour()
{
	RepManager::getInstance()->getThresholdPreview()->removePreview(this);

  QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();
  double decimation = mDecimationAdapter->getValue()/100;

  mContourAlgorithm->setInput(mSelectedImage->getImage(), outputBasePath, mSurfaceThresholdAdapter->getValue(),
  		decimation, mReduceResolution, mSmoothing, mPreserveTopology);

//  mStatusLabel->setText("<font color=orange> Generating contour... Please wait!</font>\n");
}

void SurfaceWidget::surfaceSlot()
{
	mContourAlgorithm->execute();
}

void SurfaceWidget::handleFinishedSlot()
{
  ssc::MeshPtr outputMesh = mContourAlgorithm->getOutput();
  if(!outputMesh)
    return;

  outputMesh->setColor(mDefaultColor);
//  mStatusLabel->setText("<font color=green> Done. </font>\n");

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

void SurfaceWidget::preserveSlot(bool value)
{
	mPreserveTopology = value;
}

void SurfaceWidget::imageChangedSlot(QString uid)
{
  ssc::ImagePtr image = ssc::dataManager()->getImage(uid);
  if(!image)
    return;
  mSurfaceThresholdAdapter->setValueRange(ssc::DoubleRange(image->getMin(), image->getMax(), 1));
  mSurfaceThresholdAdapter->setValue(image->getRange() / 2 + image->getMin());

  int extent[6];
  image->getBaseVtkImageData()->GetExtent(extent);
  mReduceResolutionCheckBox->setToolTip("Current input resolution: " + qstring_cast(extent[1])
  		+ " " + qstring_cast(extent[3]) + " " + qstring_cast(extent[5])
  		+ " (If checked: " + qstring_cast(extent[1]/2)+ " " + qstring_cast(extent[3]/2) + " "
  		+ qstring_cast(extent[5]/2) + ")");
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
  connect(mSurfaceThresholdAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(thresholdSlot()));

  mDecimationAdapter = ssc::DoubleDataAdapterXml::initialize("Decimation %", "",
      "Reduce number of triangles in output surface",
      80.0, ssc::DoubleRange(0, 100, 1), 0);

  QCheckBox* preserveCheckBox = new QCheckBox("Preserve mesh topology");
  preserveCheckBox->setChecked(mPreserveTopology);
  connect(preserveCheckBox, SIGNAL(toggled(bool)), this, SLOT(preserveSlot(bool)));

//  mReduceResolutionCheckBox = new QCheckBox("Reduce input volumes resolution");
  mReduceResolutionCheckBox->setChecked(mReduceResolution);
  connect(mReduceResolutionCheckBox, SIGNAL(toggled(bool)), this, SLOT(reduceResolutionSlot(bool)));

  QCheckBox* smoothingCheckBox = new QCheckBox("Smoothing");
  smoothingCheckBox->setChecked(mSmoothing);
  connect(smoothingCheckBox, SIGNAL(toggled(bool)), this, SLOT(smoothingSlot(bool)));

  QLabel* inputLabel = new QLabel("Input:");
  QLabel* outputLabel = new QLabel("Output:");

  layout->addWidget(inputLabel);
  layout->addWidget(new ssc::SpinBoxAndSliderGroupWidget(this, mSurfaceThresholdAdapter));
  layout->addWidget(mReduceResolutionCheckBox);
  layout->addWidget(outputLabel);
  layout->addWidget(new ssc::SpinBoxAndSliderGroupWidget(this, mDecimationAdapter));
  layout->addWidget(preserveCheckBox);
  layout->addWidget(smoothingCheckBox);

  return retval;
}

void SurfaceWidget::thresholdSlot()
{
	RepManager::getInstance()->getThresholdPreview()->setPreview(this, mSelectedImage->getImage(),
			mSurfaceThresholdAdapter->getValue());
}
//------------------------------------------------------------------------------



}
