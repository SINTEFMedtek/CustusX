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
#include "cxViewManager.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "cxRegistrationManager.h"
#include "sscToolManager.h"
#include "cxDataManager.h"

namespace cx
{
PatientRegistrationWidget::PatientRegistrationWidget(QWidget* parent) :
  RegistrationWidget(parent),
  mToolSampleButton(new QPushButton("Sample Tool", this))
{
  //Dock widget
  this->setObjectName("PatientRegistrationWidget");
  this->setWindowTitle("Patient Registration");

  //buttons
  mToolSampleButton->setDisabled(true);
  connect(mToolSampleButton, SIGNAL(clicked()), this, SLOT(toolSampleButtonClickedSlot()));

  //toolmanager
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot(const QString&)));

//  connect(ssc::toolManager(), SIGNAL(landmarkAdded(QString)),   this, SLOT(landmarkUpdatedSlot()));
//  connect(ssc::toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mToolSampleButton);
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);

  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
  if(dominantTool)
    this->dominantToolChangedSlot(dominantTool->getUid());
}

PatientRegistrationWidget::~PatientRegistrationWidget()
{
}

QString PatientRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Landmark based patient registration.</h3>"
      "<p>Sample points on the patient that corresponds to 3 or more landmarks already sampled in the data set. </p>"
      "<p><i>Point on the patient using a tool and click the Sample button.</i></p>"
      "</html>";
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
      (mToolToSample->getType()!=ssc::Tool::TOOL_MANUAL || DataManager::getInstance()->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.
  mToolSampleButton->setEnabled(enabled);
}

void PatientRegistrationWidget::toolSampleButtonClickedSlot()
{  
  if(!mToolToSample)
  {
    ssc::messageManager()->sendError("mToolToSample is NULL!");
    return;
  }
  //TODO What if the reference frame isnt visible?
  ssc::Transform3D lastTransform_prMt = mToolToSample->get_prMt();
  ssc::Vector3D p_pr = lastTransform_prMt.coord(ssc::Vector3D(0,0,mToolToSample->getTooltipOffset()));

  // TODO: do we want to allow sampling points not defined in image??
  if (mActiveLandmark.isEmpty() && !ssc::dataManager()->getLandmarkProperties().empty())
    mActiveLandmark = ssc::dataManager()->getLandmarkProperties().begin()->first;

  ssc::toolManager()->setLandmark(ssc::Landmark(mActiveLandmark, p_pr));

  this->nextRow();
}

void PatientRegistrationWidget::dominantToolChangedSlot(const QString& uid)
{
  if(mToolToSample && mToolToSample->getUid() == uid)
    return;

  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();

  if(mToolToSample)
    disconnect(mToolToSample.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));

  mToolToSample = dominantTool;

  if(mToolToSample)
    connect(mToolToSample.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));

  //update button
  mToolSampleButton->setText("Sample ("+qstring_cast(mToolToSample->getName())+")");
  connect(DataManager::getInstance(), SIGNAL(debugModeChanged(bool)), this, SLOT(enableToolSampleButton()));
  enableToolSampleButton();
}


void PatientRegistrationWidget::showEvent(QShowEvent* event)
{
  RegistrationWidget::showEvent(event);
  connect(ssc::toolManager(), SIGNAL(landmarkAdded(QString)),   this, SLOT(landmarkUpdatedSlot()));
  connect(ssc::toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

  viewManager()->setRegistrationMode(ssc::rsPATIENT_REGISTRATED);
}

void PatientRegistrationWidget::hideEvent(QHideEvent* event)
{
  RegistrationWidget::hideEvent(event);
  disconnect(ssc::toolManager(), SIGNAL(landmarkAdded(QString)),   this, SLOT(landmarkUpdatedSlot()));
  disconnect(ssc::toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

  viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
}

void PatientRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  RegistrationWidget::populateTheLandmarkTableWidget(image);
}

/** Return the landmarks associated with the current widget.
 */
ssc::LandmarkMap PatientRegistrationWidget::getTargetLandmarks() const
{
  return ssc::toolManager()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
ssc::Transform3D PatientRegistrationWidget::getTargetTransform() const
{
  ssc::Transform3D rMpr = *(ssc::toolManager()->get_rMpr());
  return rMpr;
}

void PatientRegistrationWidget::performRegistration()
{
  registrationManager()->doPatientRegistration();
  this->updateAvarageAccuracyLabel();
}

}//namespace cx
