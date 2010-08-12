#include "cxNavigationWidget.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include "sscMessageManager.h"
#include "cxView3D.h"
#include "cxViewManager.h"
#include "sscToolManager.h"

namespace cx
{
NavigationWidget::NavigationWidget(QWidget* parent) :
    QWidget(parent),
    mVerticalLayout(new QVBoxLayout(this)),
    mTrackingStatusGroupBox(new QGroupBox(tr("Tracking Status"), this)),
    mTrackingStatusGroupLayout(new QVBoxLayout()),
    mTrackingStatusLabel(new QLabel(tr("Configure tracker to begin..."), this)),
    mCameraGroupBox(new QGroupBox(tr("Camera Style"), this)),
    mDefaultCameraStyleRadioButton(new QRadioButton(tr("Default"), this)),
    mToolCameraStyleRadioButton(new QRadioButton(tr("Tool"), this)),
    mCameraOffsetLabel(new QLabel(tr("Camera offset: "), this)),
    mCameraOffsetSlider(new QSlider(Qt::Horizontal, this)),
    mCameraGroupLayout(new QVBoxLayout())
{
  this->setObjectName("NavigationWidget");
  this->setWindowTitle("Navigation");

  //tracking status
  mTrackingStatusGroupBox->setLayout(mTrackingStatusGroupLayout);
  mTrackingStatusGroupLayout->addWidget(mTrackingStatusLabel);

  //camera setttings
  mCameraGroupBox->setLayout(mCameraGroupLayout);
  mDefaultCameraStyleRadioButton->setChecked(true);
  mCameraOffsetSlider->setDisabled(true);
  mCameraOffsetSlider->setRange(0, 2000);
  mCameraOffsetSlider->setValue(600);
  mCameraGroupLayout->addWidget(mDefaultCameraStyleRadioButton);
  mCameraGroupLayout->addWidget(mToolCameraStyleRadioButton);
  mCameraGroupLayout->addWidget(mCameraOffsetLabel);
  mCameraGroupLayout->addWidget(mCameraOffsetSlider);
  mCameraGroupBox->setEnabled(false);

  //layout
  this->setLayout(mVerticalLayout);
  mVerticalLayout->addWidget(mTrackingStatusGroupBox);
  mVerticalLayout->addWidget(mCameraGroupBox);
  mVerticalLayout->addStretch();

  //connections
  connect(mDefaultCameraStyleRadioButton, SIGNAL(toggled(bool)),
          this, SLOT(radioButtonToggledSlot(bool)));
  connect(mToolCameraStyleRadioButton, SIGNAL(toggled(bool)),
          this, SLOT(radioButtonToggledSlot(bool)));

  connect(ssc::toolManager(), SIGNAL(configured()),
          this, SLOT(trackingConfiguredSlot()));
  connect(ssc::toolManager(), SIGNAL(initialized()),
          this, SLOT(trackingInitializedSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()),
          this, SLOT(trackingStartedSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()),
          this, SLOT(trackingStoppedSlot()));
}
NavigationWidget::~NavigationWidget()
{}
void NavigationWidget::radioButtonToggledSlot(bool checked)
{
  if(this->sender() == mDefaultCameraStyleRadioButton)
  {
    if(checked)
    {
      mCameraOffsetSlider->setDisabled(true);

      View3D* view3D_1Ptr = viewManager()->get3DView();
      view3D_1Ptr->setCameraStyle(View3D::DEFAULT_STYLE);
      ssc::messageManager()->sendInfo("Default camera selected");
    }
  }
  else if(this->sender() == mToolCameraStyleRadioButton)
  {
    View3D* view3D_1Ptr = viewManager()->get3DView();
    if(checked)
    {
      mCameraOffsetSlider->setEnabled(true);
      view3D_1Ptr->setCameraStyle(View3D::TOOL_STYLE, mCameraOffsetSlider->value());
      connect(mCameraOffsetSlider, SIGNAL(valueChanged(int)),
                 view3D_1Ptr, SLOT(setCameraOffsetSlot(int)));
      ssc::messageManager()->sendInfo("Tool camera selected");
    }else
    {
      if(view3D_1Ptr)
      {
        disconnect(mCameraOffsetSlider, SIGNAL(valueChanged(int)),
                   view3D_1Ptr, SLOT(setCameraOffsetSlot(int)));
      }
    }

  }
}
void NavigationWidget::trackingConfiguredSlot()
{
  mCameraGroupBox->setEnabled(true);
  mTrackingStatusLabel->setText("Configured");
}
void NavigationWidget::trackingInitializedSlot()
{
  mTrackingStatusLabel->setText("Initialized");
}
void NavigationWidget::trackingStartedSlot()
{
  mTrackingStatusLabel->setText("Tracking...");
}
void NavigationWidget::trackingStoppedSlot()
{
  mTrackingStatusLabel->setText("Tracking stopped...");
}
}
