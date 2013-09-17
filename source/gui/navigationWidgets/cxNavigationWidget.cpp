#include "cxNavigationWidget.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QToolBar>
#include "sscMessageManager.h"
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
    mCameraGroupLayout(new QVBoxLayout())
{

  //camera setttings
  mCameraGroupBox->setLayout(mCameraGroupLayout);

  QToolBar* toolBar = new QToolBar(this);
  mCameraGroupLayout->addWidget(toolBar);
  toolBar->addActions(this->getCameraStyle()->createInteractorStyleActionGroup()->actions());

  QWidget* toolOffsetWidget = new SliderGroupWidget(this, DoubleDataAdapterActiveToolOffset::create());

  //layout
  this->setLayout(mVerticalLayout);
  mVerticalLayout->addWidget(mCameraGroupBox);
  mVerticalLayout->addWidget(toolOffsetWidget);
  mVerticalLayout->addStretch();
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

}
