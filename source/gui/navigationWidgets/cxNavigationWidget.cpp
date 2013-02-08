#include "cxNavigationWidget.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QToolBar>
#include "sscMessageManager.h"
#include "cxView3D.h"
#include "cxViewManager.h"
#include "sscToolManager.h"
#include "sscDoubleWidgets.h"
#include "cxDataInterface.h"
#include "cxDataAdapterHelper.h"

namespace cx
{
NavigationWidget::NavigationWidget(QWidget* parent) :
    BaseWidget(parent, "NavigationWidget", "Navigation Properties"),
    mVerticalLayout(new QVBoxLayout(this)),
    mCameraGroupBox(new QGroupBox(tr("Camera Style"), this)),
//    mDefaultCameraStyleRadioButton(new QRadioButton(tr("Default"), this)),
//    mToolCameraStyleRadioButton(new QRadioButton(tr("Tool"), this)),
//    mAngledToolCameraStyleRadioButton(new QRadioButton(tr("Angled tool"), this)),
//    mCameraOffsetLabel(new QLabel(tr("Camera offset: "), this)),
//    mCameraOffsetSlider(new QSlider(Qt::Horizontal, this)),
    mCameraGroupLayout(new QVBoxLayout())
{
//  mCameraStyle.reset(new CameraStyle());

//  mDefaultCameraStyleRadioButton->setToolTip("Fixed camera (Move it with the mouse");
//  mToolCameraStyleRadioButton->setToolTip("Camera following tool");
//  mAngledToolCameraStyleRadioButton->setToolTip("Camera following tool (Placed at an angle of 20 degrees)");

  //camera setttings
  mCameraGroupBox->setLayout(mCameraGroupLayout);
//  mDefaultCameraStyleRadioButton->setChecked(true);
//  mCameraOffsetSlider->setDisabled(true);
//  mCameraOffsetSlider->setRange(0, 2000);
//  mCameraOffsetSlider->setValue(600);
//  mCameraGroupLayout->addWidget(mDefaultCameraStyleRadioButton);
//  mCameraGroupLayout->addWidget(mToolCameraStyleRadioButton);
//  mCameraGroupLayout->addWidget(mAngledToolCameraStyleRadioButton);
//  mCameraGroupLayout->addWidget(mCameraOffsetLabel);
//  mCameraGroupLayout->addWidget(mCameraOffsetSlider);
//  mCameraGroupBox->setEnabled(false);//Allow changing camera style even if tracking isn't configured. Should allow testing with manual tool

  QToolBar* toolBar = new QToolBar(this);
  mCameraGroupLayout->addWidget(toolBar);
  toolBar->addActions(this->getCameraStyle()->createInteractorStyleActionGroup()->actions());

  QWidget* toolOffsetWidget = new ssc::SliderGroupWidget(this, DoubleDataAdapterActiveToolOffset::create());
//  QWidget* toolOffsetWidget2 = createDataWidget(this, DoubleDataAdapterActiveToolOffset::create());

  //layout
  this->setLayout(mVerticalLayout);
  mVerticalLayout->addWidget(mCameraGroupBox);
  mVerticalLayout->addWidget(toolOffsetWidget);
//  mVerticalLayout->addWidget(toolOffsetWidget2);
  mVerticalLayout->addStretch();

//  //connections
//  connect(mDefaultCameraStyleRadioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonToggledSlot(bool)));
//  connect(mToolCameraStyleRadioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonToggledSlot(bool)));
//  connect(mAngledToolCameraStyleRadioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonToggledSlot(bool)));

//  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(trackingConfiguredSlot()));
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

CameraStylePtr NavigationWidget::getCameraStyle()
{
	return viewManager()->getCameraStyle();
}

//// legg inn listen to activeLayoutChanged her.... og reset style
//void NavigationWidget::radioButtonToggledSlot(bool checked)
//{
//  if(this->sender() == mDefaultCameraStyleRadioButton)
//  {
//    if(checked)
//    {
////      mCameraOffsetSlider->setDisabled(true);
////      this->getCameraStyle()->setCameraStyle(CameraStyle::DEFAULT_STYLE);
//    }
//  }
//  else
//  {
//		if (this->sender() == mToolCameraStyleRadioButton)
//			this->getCameraStyle()->setCameraStyle(CameraStyle::TOOL_STYLE);
//		else if (this->sender() == mAngledToolCameraStyleRadioButton)
//			this->getCameraStyle()->setCameraStyle(CameraStyle::ANGLED_TOOL_STYLE);
////		if (checked)
////		{
////			mCameraOffsetSlider->setEnabled(true);
////			//mCameraStyle->setCameraStyle(CameraStyle::TOOL_STYLE, mCameraOffsetSlider->value());
////			connect(mCameraOffsetSlider, SIGNAL(valueChanged(int)), this->getCameraStyle().get(), SLOT(setCameraOffsetSlot(int)));
////		}
////		else
////			disconnect(mCameraOffsetSlider, SIGNAL(valueChanged(int)), this->getCameraStyle().get(), SLOT(setCameraOffsetSlot(int)));
//	}
//}

//void NavigationWidget::trackingConfiguredSlot()
//{
//  mCameraGroupBox->setEnabled(true);
//}
}
