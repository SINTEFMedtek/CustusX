/*
 * cxRegistrationBaseWidget.cpp
 *
 *  \date Jun 16, 2011
 *      \author christiana
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
