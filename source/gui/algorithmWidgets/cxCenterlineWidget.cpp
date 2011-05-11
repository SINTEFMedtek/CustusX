/*
 * cxCenterlineWidget.cpp
 *
 *  Created on: Apr 29, 2011
 *      Author: christiana
 */

#include <cxCenterlineWidget.h>

#include "sscLabeledComboBoxWidget.h"
#include "cxDataInterface.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"


namespace cx
{

CenterlineWidget::CenterlineWidget(QWidget* parent) :
  BaseWidget(parent, "CenterlineWidget", "CenterlineWidget"),
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
  if(mCenterlineAlgorithm.setInput(mSelectedImage->getImage(), outputBasePath))
  {
    //Only print text if the input is in the correct format
    mStatusLabel->setText("<font color=orange> Generating centerline... Please wait!</font>\n");
  }
  else
  {
    mStatusLabel->setText("<font color=red> Incorrect input</font>\n");
  }
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


}
