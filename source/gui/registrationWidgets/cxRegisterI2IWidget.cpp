/*
 * cxRegisterI2IWidget.cpp
 *
 *  Created on: Apr 29, 2011
 *      Author: christiana
 */

#include <cxRegisterI2IWidget.h>

//#include <boost/bind.hpp>
//
//#include <QTreeWidget>
//#include <QTreeWidgetItem>
//#include <QStringList>
//#include <QGridLayout>
//#include <QCheckBox>
//
//#include <vtkPolyData.h>
//
//#include "sscUtilHelpers.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscTypeConversions.h"
#include "sscImage.h"
//#include "sscMesh.h"
#include "sscDataManager.h"
//#include "sscMessageManager.h"
//#include "sscDoubleWidgets.h"
#include "sscLabeledComboBoxWidget.h"
//#include "cxDataInterface.h"
//#include "cxVolumePropertiesWidget.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxFrameTreeWidget.h"
#include "cxDataInterface.h"
//#include "cxDataLocations.h"
#include "cxSeansVesselRegistrationWidget.h"

namespace cx
{


RegisterI2IWidget::RegisterI2IWidget(QWidget* parent) :
    WhatsThisWidget(parent, "RegisterI2IWidget", "Register Image2Image"),
    mSeansVesselRegsitrationWidget(new SeansVesselRegistrationWidget(this))
{
  connect(registrationManager(), SIGNAL(fixedDataChanged(QString)), this, SLOT(fixedImageSlot(QString)));
  connect(registrationManager(), SIGNAL(movingDataChanged(QString)), this, SLOT(movingImageSlot(QString)));

  QVBoxLayout* topLayout = new QVBoxLayout(this);
  QGridLayout* layout = new QGridLayout();
  topLayout->addLayout(layout);

  layout->addWidget(mSeansVesselRegsitrationWidget);
  layout->addWidget(new QLabel("Parent frame tree status:"), 3, 0);
  layout->addWidget(new FrameTreeWidget(this), 4, 0);
}

RegisterI2IWidget::~RegisterI2IWidget()
{}

QString RegisterI2IWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Registration of vessel segments to eachother.</h3>"
      "<p><i>Press the button to perform vessel based registration between image 1 and image 2s centerlines.</i></p>"
      "</html>";
}

void RegisterI2IWidget::fixedImageSlot(QString uid)
{
  mSeansVesselRegsitrationWidget->fixedImageSlot(uid);
}

void RegisterI2IWidget::movingImageSlot(QString uid)
{
  mSeansVesselRegsitrationWidget->movingImageSlot(uid);
}

}
