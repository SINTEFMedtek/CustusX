/*
 * cxResampleWidget.cpp
 *
 *  \date Apr 29, 2011
 *      \author christiana
 */

#include <cxResampleWidget.h>

#include "sscTypeConversions.h"
#include "sscDataManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxPatientData.h"
#include "cxDataInterface.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscDoubleWidgets.h"
#include "cxPatientService.h"

namespace cx
{

ResampleWidget::ResampleWidget(QWidget* parent) :
  BaseWidget(parent, "ResampleWidget", "Resample"),
  mStatusLabel(new QLabel(""))
{
  connect(&mResampleAlgorithm, SIGNAL(finished()), this, SLOT(handleFinishedSlot()));
  connect(&mResampleAlgorithm, SIGNAL(aboutToStart()), this, SLOT(preprocessResampler()));

  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(dataChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);

  mReferenceImage = SelectImageStringDataAdapter::New();
  mReferenceImage->setValueName("Select reference: ");
  ssc::LabeledComboBoxWidget* referenceImageComboBox = new ssc::LabeledComboBoxWidget(this, mReferenceImage);

  QPushButton* resampleButton = new QPushButton("Resample", this);
  connect(resampleButton, SIGNAL(clicked()), this, SLOT(resampleSlot()));

  mMargin = ssc::DoubleDataAdapterXml::initialize("Margin", "",
                                                 "mm Margin added to ref image bounding box",
                                                  5.0, ssc::DoubleRange(0, 50, 1), 1);
//  margin->setInternal2Display(double factor);
  ;
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  topLayout->addWidget(selectImageComboBox, 0, 0);
  topLayout->addWidget(referenceImageComboBox, 1, 0);
  topLayout->addWidget(new ssc::SpinBoxGroupWidget(this, mMargin), 2, 0);
  topLayout->addWidget(mStatusLabel, 3, 0);
  topLayout->addWidget(resampleButton, 4, 0);
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

void ResampleWidget::preprocessResampler()
{
  QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();
  double margin = mMargin->getValue();

  mResampleAlgorithm.setInput(mSelectedImage->getImage(), mReferenceImage->getImage(), outputBasePath, margin);
  mStatusLabel->setText("<font color=orange> Generating resampling... Please wait!</font>\n");
}

void ResampleWidget::resampleSlot()
{
	mResampleAlgorithm.execute();
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


}
