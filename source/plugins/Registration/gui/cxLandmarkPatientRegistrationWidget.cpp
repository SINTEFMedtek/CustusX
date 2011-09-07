#include "cxLandmarkPatientRegistrationWidget.h"

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
#include "sscLabeledComboBoxWidget.h"

namespace cx
{
LandmarkPatientRegistrationWidget::LandmarkPatientRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle) :
  LandmarkRegistrationWidget(regManager, parent, objectName, windowTitle),
  mToolSampleButton(new QPushButton("Sample Tool", this))
{
	mFixedDataAdapter.reset(new RegistrationFixedImageStringDataAdapter(regManager));

  //buttons
  mToolSampleButton->setDisabled(true);
  connect(mToolSampleButton, SIGNAL(clicked()), this, SLOT(toolSampleButtonClickedSlot()));

  mRemoveLandmarkButton = new QPushButton("Clear", this);
  mRemoveLandmarkButton->setToolTip("Clear selected landmark");
//  mRemoveLandmarkButton->setDisabled(true);
  connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

  mRegisterButton = new QPushButton("Register", this);
  mRegisterButton->setToolTip("Perform registration");
  connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

  //toolmanager
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot(const QString&)));

  //layout
  mVerticalLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mFixedDataAdapter));
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mToolSampleButton);
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);
//  mVerticalLayout->addWidget(mRegisterButton);
//  mVerticalLayout->addWidget(mRemoveLandmarkButton);

  QHBoxLayout* buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(mRegisterButton);
  buttonsLayout->addWidget(mRemoveLandmarkButton);
  mVerticalLayout->addLayout(buttonsLayout);


  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
  if(dominantTool)
    this->dominantToolChangedSlot(dominantTool->getUid());
}

LandmarkPatientRegistrationWidget::~LandmarkPatientRegistrationWidget()
{
}

QString LandmarkPatientRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Landmark based patient registration.</h3>"
      "<p>Sample points on the patient that corresponds to 3 or more landmarks already sampled in the data set. </p>"
      "<p><i>Point on the patient using a tool and click the Sample button.</i></p>"
      "<p>Landmark patient registration will move the patient into the global coordinate system (r).</p>"
      "</html>";
}

void LandmarkPatientRegistrationWidget::registerSlot()
{
	this->performRegistration();
}

void LandmarkPatientRegistrationWidget::activeImageChangedSlot()
{
  LandmarkRegistrationWidget::activeImageChangedSlot();
}

void LandmarkPatientRegistrationWidget::toolVisibleSlot(bool visible)
{
  enableToolSampleButton();
}

void LandmarkPatientRegistrationWidget::enableToolSampleButton()
{
  bool enabled = false;
  enabled = mToolToSample &&
      mToolToSample->getVisible() &&
      (mToolToSample->getType()!=ssc::Tool::TOOL_MANUAL || DataManager::getInstance()->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.
  mToolSampleButton->setEnabled(enabled);
}

void LandmarkPatientRegistrationWidget::toolSampleButtonClickedSlot()
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
  ssc::messageManager()->playSampleSound();

  this->nextRow();
}

void LandmarkPatientRegistrationWidget::dominantToolChangedSlot(const QString& uid)
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
  mToolSampleButton->setText("Sample "+qstring_cast(mToolToSample->getName()));
  connect(DataManager::getInstance(), SIGNAL(debugModeChanged(bool)), this, SLOT(enableToolSampleButton()));
  this->enableToolSampleButton();
}


void LandmarkPatientRegistrationWidget::showEvent(QShowEvent* event)
{
  LandmarkRegistrationWidget::showEvent(event);
  connect(ssc::toolManager(), SIGNAL(landmarkAdded(QString)),   this, SLOT(landmarkUpdatedSlot()));
  connect(ssc::toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

  viewManager()->setRegistrationMode(ssc::rsPATIENT_REGISTRATED);
}

void LandmarkPatientRegistrationWidget::hideEvent(QHideEvent* event)
{
  LandmarkRegistrationWidget::hideEvent(event);
  disconnect(ssc::toolManager(), SIGNAL(landmarkAdded(QString)),   this, SLOT(landmarkUpdatedSlot()));
  disconnect(ssc::toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

  viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
}

void LandmarkPatientRegistrationWidget::removeLandmarkButtonClickedSlot()
{
	ssc::toolManager()->removeLandmark(mActiveLandmark);
  this->nextRow();
}

void LandmarkPatientRegistrationWidget::cellClickedSlot(int row, int column)
{
  LandmarkRegistrationWidget::cellClickedSlot(row, column);

  mRemoveLandmarkButton->setEnabled(true);
}

void LandmarkPatientRegistrationWidget::populateTheLandmarkTableWidget()
{
  LandmarkRegistrationWidget::populateTheLandmarkTableWidget();

  std::vector<ssc::Landmark> landmarks =  this->getAllLandmarks();
  mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
}

/** Return the landmarks associated with the current widget.
 */
ssc::LandmarkMap LandmarkPatientRegistrationWidget::getTargetLandmarks() const
{
  return ssc::toolManager()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
ssc::Transform3D LandmarkPatientRegistrationWidget::getTargetTransform() const
{
  ssc::Transform3D rMpr = *(ssc::toolManager()->get_rMpr());
  return rMpr;
}

void LandmarkPatientRegistrationWidget::performRegistration()
{
  if(!mManager->getFixedData())
    mManager->setFixedData(ssc::dataManager()->getActiveImage());

  mManager->doPatientRegistration();

  this->updateAvarageAccuracyLabel();
}


}//namespace cx
