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
#include "sscDoubleWidgets.h"
#include "cxDataInterface.h"

namespace cx
{
NavigationWidget::NavigationWidget(QWidget* parent) :
    WhatsThisWidget(parent, "NavigationWidget", "Navigation Properties"),
    mVerticalLayout(new QVBoxLayout(this)),
    mCameraGroupBox(new QGroupBox(tr("Camera Style"), this)),
    mDefaultCameraStyleRadioButton(new QRadioButton(tr("Default"), this)),
    mToolCameraStyleRadioButton(new QRadioButton(tr("Tool"), this)),
    mCameraOffsetLabel(new QLabel(tr("Camera offset: "), this)),
    mCameraOffsetSlider(new QSlider(Qt::Horizontal, this)),
    mCameraGroupLayout(new QVBoxLayout())
{
  mCameraStyle.reset(new CameraStyle());

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

  QWidget* toolOffsetWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterActiveToolOffset));

  //layout
  this->setLayout(mVerticalLayout);
  mVerticalLayout->addWidget(mCameraGroupBox);
  mVerticalLayout->addWidget(toolOffsetWidget);
  mVerticalLayout->addStretch();

  //connections
  connect(mDefaultCameraStyleRadioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonToggledSlot(bool)));
  connect(mToolCameraStyleRadioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonToggledSlot(bool)));

  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(trackingConfiguredSlot()));
}
NavigationWidget::~NavigationWidget()
{}

QString NavigationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Camera navigation.</h3>"
      "<p>Lets you select if the camera should follow a tool.</p>"
      "<p><i>Click the options to select camera following style.</i></p>"
      "</html>";
}

// legg inn listen to activeLayoutChanged her.... og reset style
void NavigationWidget::radioButtonToggledSlot(bool checked)
{
  if(this->sender() == mDefaultCameraStyleRadioButton)
  {
    if(checked)
    {
      mCameraOffsetSlider->setDisabled(true);
      mCameraStyle->setCameraStyle(CameraStyle::DEFAULT_STYLE);
    }
  }
  else if(this->sender() == mToolCameraStyleRadioButton)
  {
    if(checked)
    {
      mCameraOffsetSlider->setEnabled(true);
      mCameraStyle->setCameraStyle(CameraStyle::TOOL_STYLE, mCameraOffsetSlider->value());
      connect(mCameraOffsetSlider, SIGNAL(valueChanged(int)), mCameraStyle.get(), SLOT(setCameraOffsetSlot(int)));
    }
    else
      disconnect(mCameraOffsetSlider, SIGNAL(valueChanged(int)), mCameraStyle.get(), SLOT(setCameraOffsetSlot(int)));
  }
}

void NavigationWidget::trackingConfiguredSlot()
{
  mCameraGroupBox->setEnabled(true);
}
}
