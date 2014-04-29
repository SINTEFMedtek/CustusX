/*
 * cxRegisterI2IWidget.cpp
 *
 *  \date Apr 29, 2011
 *      \author christiana
 */

#include <cxRegisterI2IWidget.h>

#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxTypeConversions.h"
#include "cxImage.h"
#include "cxDataManager.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxPatientData.h"
#include "cxFrameTreeWidget.h"
#include "cxDataInterface.h"
#include "cxSeansVesselRegistrationWidget.h"

namespace cx
{


RegisterI2IWidget::RegisterI2IWidget(RegistrationManagerPtr regManager, QWidget* parent) :
    RegistrationBaseWidget(regManager, parent, "RegisterI2IWidget", "Register Image2Image"),
    mSeansVesselRegsitrationWidget(new SeansVesselRegistrationWidget(regManager, this))
{
//  connect(registrationManager(), SIGNAL(fixedDataChanged(QString)), this, SLOT(fixedImageSlot(QString)));
//  connect(registrationManager(), SIGNAL(movingDataChanged(QString)), this, SLOT(movingImageSlot(QString)));

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
//
//void RegisterI2IWidget::fixedImageSlot(QString uid)
//{
//  mSeansVesselRegsitrationWidget->fixedImageSlot(uid);
//}
//
//void RegisterI2IWidget::movingImageSlot(QString uid)
//{
//  mSeansVesselRegsitrationWidget->movingImageSlot(uid);
//}

}
