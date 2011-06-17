/*
 * cxRegistrationBaseWidget.cpp
 *
 *  Created on: Jun 16, 2011
 *      Author: christiana
 */

#include "cxRegistrationBaseWidget.h"

namespace cx
{

RegistrationBaseWidget::RegistrationBaseWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle) :
  BaseWidget(parent, objectName, windowTitle),
  mManager(regManager)
{
}

}
