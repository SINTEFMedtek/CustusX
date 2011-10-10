/*
 * cxSurfaceWidget.cpp
 *
 *  Created on: Apr 29, 2011
 *      Author: christiana
 */

#include <cxSurfaceWidget.h>

#include "sscImage.h"
#include "sscMesh.h"
#include "sscDataManager.h"
#include "sscDoubleWidgets.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxPatientData.h"
#include "cxDataInterface.h"
#include "cxPatientService.h"

namespace cx
{

SurfaceWidget::SurfaceWidget(QWidget* parent) :
    BaseWidget(parent, "SurfaceWidget", "Surface"),
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
	patientService()->getThresholdPreview()->removePreview(this);

  QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();
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
  connect(mSurfaceThresholdAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(thresholdSlot()));

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

void SurfaceWidget::thresholdSlot()
{
	patientService()->getThresholdPreview()->setPreview(this, mSelectedImage->getImage(),
			mSurfaceThresholdAdapter->getValue());
}
//------------------------------------------------------------------------------



}
