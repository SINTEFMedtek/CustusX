#include "cxPatientRegistrationWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QSlider>
#include <QGridLayout>
#include <QSpinBox>
#include <vtkDoubleArray.h>
#include <sscVector3D.h>
#include <sscVolumetricRep.h>
#include "sscTypeConversions.h"
#include "cxLandmarkRep.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
#include "cxToolManager.h"
#include "cxDataManager.h"
#include "cxRegistrationHistoryWidget.h"

namespace cx
{
PatientRegistrationWidget::PatientRegistrationWidget(QWidget* parent) :
  RegistrationWidget(parent),
  mToolSampleButton(new QPushButton("Sample Tool", this)),
  mOffsetLabel(new QLabel(QString("Manual offset:"), this)),
  mOffsetWidget(new QWidget(this)),
  mOffsetsGridLayout(new QGridLayout(mOffsetWidget)),
  mXLabel(new QLabel(QString("X "), this)),
  mYLabel(new QLabel(QString("Y "), this)),
  mZLabel(new QLabel(QString("Z "), this)),
  mXOffsetSlider(new QSlider(Qt::Horizontal, this)),
  mYOffsetSlider(new QSlider(Qt::Horizontal, this)),
  mZOffsetSlider(new QSlider(Qt::Horizontal, this)),
  mXSpinBox(new QSpinBox(mOffsetWidget)),
  mYSpinBox(new QSpinBox(mOffsetWidget)),
  mZSpinBox(new QSpinBox(mOffsetWidget)),
  mResetOffsetButton(new QPushButton("Clear offset", this)),
  mMinValue(-200),
  mMaxValue(200),
  mDefaultValue(0)
{
  //Dock widget
  this->setWindowTitle("Patient Registration");

  //buttons
  mToolSampleButton->setDisabled(true);
  connect(mToolSampleButton, SIGNAL(clicked()), this, SLOT(toolSampleButtonClickedSlot()));
  mResetOffsetButton->setDisabled(true);
  connect(mResetOffsetButton, SIGNAL(clicked()), this, SLOT(resetOffsetSlot()));

  //toolmanager
  connect(toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot(const std::string&)));

  connect(toolManager(), SIGNAL(landmarkAdded(std::string)),   this, SLOT(landmarkUpdatedSlot(std::string)));
  connect(toolManager(), SIGNAL(landmarkRemoved(std::string)), this, SLOT(landmarkUpdatedSlot(std::string)));

  //registrationmanager
  connect(RegistrationManager::getInstance(), SIGNAL(patientRegistrationPerformed()), this, SLOT(activateManualRegistrationFieldSlot()));
  connect(ToolManager::getInstance(), SIGNAL(rMprChanged()), this, SLOT(activateManualRegistrationFieldSlot()));

  //sliders
  mXOffsetSlider->setRange(mMinValue,mMaxValue);
  mXOffsetSlider->setValue(mDefaultValue);
  mYOffsetSlider->setRange(mMinValue,mMaxValue);
  mYOffsetSlider->setValue(mDefaultValue);
  mZOffsetSlider->setRange(mMinValue,mMaxValue);
  mZOffsetSlider->setValue(mDefaultValue);

  //spinboxes
  mXSpinBox->setRange(mMinValue,mMaxValue);
  mXSpinBox->setValue(mDefaultValue);
  mYSpinBox->setRange(mMinValue,mMaxValue);
  mYSpinBox->setValue(mDefaultValue);
  mZSpinBox->setRange(mMinValue,mMaxValue);
  mZSpinBox->setValue(mDefaultValue);

  //connect sliders to the spinbox
  connect(mXOffsetSlider, SIGNAL(valueChanged(int)),
          mXSpinBox, SLOT(setValue(int)));
  connect(mYOffsetSlider, SIGNAL(valueChanged(int)),
          mYSpinBox, SLOT(setValue(int)));
  connect(mZOffsetSlider, SIGNAL(valueChanged(int)),
          mZSpinBox, SLOT(setValue(int)));
  connect(mXSpinBox, SIGNAL(valueChanged(int)),
          mXOffsetSlider, SLOT(setValue(int)));
  connect(mYSpinBox, SIGNAL(valueChanged(int)),
          mYOffsetSlider, SLOT(setValue(int)));
  connect(mZSpinBox, SIGNAL(valueChanged(int)),
          mZOffsetSlider, SLOT(setValue(int)));

  //connect sliders registrationmanager,
  //spinboxes are connected to the sliders so no need to connect them to the
  //registration manager as well
  connect(mXOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffsetSlot(int)));
  connect(mYOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffsetSlot(int)));
  connect(mZOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffsetSlot(int)));

  //layout
  mOffsetsGridLayout->addWidget(mXLabel, 0, 0);
  mOffsetsGridLayout->addWidget(mYLabel, 1, 0);
  mOffsetsGridLayout->addWidget(mZLabel, 2, 0);
  mOffsetsGridLayout->addWidget(mXOffsetSlider, 0, 1);
  mOffsetsGridLayout->addWidget(mYOffsetSlider, 1, 1);
  mOffsetsGridLayout->addWidget(mZOffsetSlider, 2, 1);
  mOffsetsGridLayout->addWidget(mXSpinBox, 0, 2);
  mOffsetsGridLayout->addWidget(mYSpinBox, 1, 2);
  mOffsetsGridLayout->addWidget(mZSpinBox, 2, 2);

  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mToolSampleButton);
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);
  mVerticalLayout->addWidget(mOffsetLabel);
  mVerticalLayout->addWidget(mOffsetWidget);
  mVerticalLayout->addWidget(mResetOffsetButton);
  mVerticalLayout->addWidget(new RegistrationHistoryWidget(this));

  mOffsetWidget->setDisabled(true);

  ssc::ToolPtr dominantTool = toolManager()->getDominantTool();
  if(dominantTool)
    this->dominantToolChangedSlot(dominantTool->getUid());
}

PatientRegistrationWidget::~PatientRegistrationWidget()
{
}

void PatientRegistrationWidget::activeImageChangedSlot()
{
  RegistrationWidget::activeImageChangedSlot();
}

void PatientRegistrationWidget::toolVisibleSlot(bool visible)
{
  enableToolSampleButton();
}

void PatientRegistrationWidget::enableToolSampleButton()
{
  bool enabled = false;
  enabled = mToolToSample &&
      mToolToSample->getVisible() &&
      (mToolToSample->getType()!=ssc::Tool::TOOL_MANUAL || dataManager()->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.
  mToolSampleButton->setEnabled(enabled);
}

void PatientRegistrationWidget::toolSampleButtonClickedSlot()
{  
  if(!mToolToSample)
  {
    messageManager()->sendError("mToolToSample is NULL!");
    return;
  }
  //TODO What if the reference frame isnt visible?
  ssc::Transform3D lastTransform_prMt = mToolToSample->get_prMt();
  ssc::Vector3D p_pr = lastTransform_prMt.coord(ssc::Vector3D(0,0,mToolToSample->getTooltipOffset()));

  // TODO: do we want to allow sampling points not defined in image??
  if (mActiveLandmark.empty() && !dataManager()->getLandmarkProperties().empty())
    mActiveLandmark = dataManager()->getLandmarkProperties().begin()->first;

  toolManager()->setLandmark(ssc::Landmark(mActiveLandmark, p_pr));

  this->nextRow();
}

void PatientRegistrationWidget::dominantToolChangedSlot(const std::string& uid)
{
  if(mToolToSample && mToolToSample->getUid() == uid)
    return;

  ssc::ToolPtr dominantTool = toolManager()->getDominantTool();

  if(mToolToSample)
    disconnect(mToolToSample.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));

  mToolToSample = dominantTool;

  if(mToolToSample)
    connect(mToolToSample.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));

  //update button
  mToolSampleButton->setText("Sample ("+qstring_cast(mToolToSample->getName())+")");
  connect(dataManager(), SIGNAL(debugModeChanged(bool)), this, SLOT(enableToolSampleButton()));
  enableToolSampleButton();
}

void PatientRegistrationWidget::resetOffsetSlot()
{
  if(mXOffsetSlider->value() != mDefaultValue ||
     mYOffsetSlider->value() != mDefaultValue ||
     mZOffsetSlider->value() != mDefaultValue)
  {
    mXOffsetSlider->setValue(mDefaultValue);
    mYOffsetSlider->setValue(mDefaultValue);
    mZOffsetSlider->setValue(mDefaultValue);
    mResetOffsetButton->setDisabled(true);
    mLandmarkTableWidget->setEnabled(true);
  }
}

void PatientRegistrationWidget::setOffsetSlot(int value)
{
  mResetOffsetButton->setEnabled(true);
  mLandmarkTableWidget->setDisabled(true);
  
  vtkMatrix4x4* offsetMatrix = vtkMatrix4x4::New(); //identity
  offsetMatrix->SetElement(0, 3, mXOffsetSlider->value());
  offsetMatrix->SetElement(1, 3, mYOffsetSlider->value());
  offsetMatrix->SetElement(2, 3, mZOffsetSlider->value());
  
  ssc::Transform3DPtr offsetPtr(new ssc::Transform3D(offsetMatrix));
  registrationManager()->setManualPatientRegistrationOffsetSlot(offsetPtr);
}

void PatientRegistrationWidget::activateManualRegistrationFieldSlot()
{
  mOffsetWidget->setEnabled(true);
}

void PatientRegistrationWidget::showEvent(QShowEvent* event)
{
  RegistrationWidget::showEvent(event);
}

void PatientRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  RegistrationWidget::populateTheLandmarkTableWidget(image);
}

/** Return the landmarks associated with the current widget.
 */
ssc::LandmarkMap PatientRegistrationWidget::getTargetLandmarks() const
{
  return toolManager()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
ssc::Transform3D PatientRegistrationWidget::getTargetTransform() const
{
  ssc::Transform3D rMpr = *(toolManager()->get_rMpr());
  return rMpr;
}

void PatientRegistrationWidget::performRegistration()
{
  registrationManager()->doPatientRegistration();
  this->updateAvarageAccuracyLabel();
}

}//namespace cx
