/*
 * cxCenterlineWidget.cpp
 *
 *  \date Apr 29, 2011
 *      \author christiana
 */

#include <cxCenterlineWidget.h>

#include "sscLabeledComboBoxWidget.h"
#include "cxDataInterface.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxTimedAlgorithmProgressBar.h"

namespace cx
{

CenterlineWidget::CenterlineWidget(QWidget* parent) :
  BaseWidget(parent, "CenterlineWidget", "CenterlineWidget"),
  mFindCenterlineButton(new QPushButton("Find centerline"))
//  mStatusLabel(new QLabel(""))
{
	mCenterlineAlgorithm.reset(new Centerline);
  connect(mCenterlineAlgorithm.get(), SIGNAL(finished()), this, SLOT(handleFinishedSlot()));
  connect(mCenterlineAlgorithm.get(), SIGNAL(aboutToStart()), this, SLOT(preprocessResampler()));

  QVBoxLayout* layout = new QVBoxLayout(this);

  mSelectedImage = SelectImageStringDataAdapter::New();
  mSelectedImage->setValueName("Select input: ");
  connect(mSelectedImage.get(), SIGNAL(imageChanged(QString)), this, SIGNAL(inputImageChanged(QString)));
  ssc::LabeledComboBoxWidget* selectImageComboBox = new ssc::LabeledComboBoxWidget(this, mSelectedImage);

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mCenterlineAlgorithm);

  layout->addWidget(selectImageComboBox);
  layout->addWidget(mFindCenterlineButton);
//  layout->addWidget(mStatusLabel);
  layout->addWidget(mTimedAlgorithmProgressBar);
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
  mCenterlineAlgorithm->setDefaultColor(color);
}

void CenterlineWidget::preprocessResampler()
{
	  QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();
	  if(mCenterlineAlgorithm->setInput(mSelectedImage->getImage(), outputBasePath))
	  {
	  }
	  else
	  {
		  ssc::messageManager()->sendWarning("Centerline: Incorrect input");
	  }
}

void CenterlineWidget::findCenterlineSlot()
{
	mCenterlineAlgorithm->execute();
}

void CenterlineWidget::handleFinishedSlot()
{
  ssc::DataPtr centerlineImage = mCenterlineAlgorithm->getOutput();
  if(!centerlineImage)
    return;

  emit outputImageChanged(centerlineImage->getUid());
}

//------------------------------------------------------------------------------


}
